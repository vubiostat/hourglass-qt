#include <QtDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include "activity.h"
#include "tag.h"

// Static members
const QString Activity::distinctNamesQuery = QString(
    "SELECT DISTINCT activities.name, projects.name AS project_name "
    "FROM activities "
    "LEFT JOIN projects ON activities.project_id = projects.id "
    "ORDER BY activities.name, projects.name");

const QString Activity::stopCurrentQuery = QString(
    "UPDATE activities SET ended_at = datetime('now', 'localtime') "
    "WHERE ended_at IS NULL");

const QString Activity::deleteShortQuery = QString(
    "DELETE FROM activities "
    "WHERE ended_at IS NOT NULL AND ("
      "CAST(strftime('%s', ended_at, 'localtime') AS INTEGER) - "
      "CAST(strftime('%s', started_at, 'localtime') AS INTEGER) < 60"
    ")");

QList<Activity> Activity::find(QString conditions)
{
  return Model::find<Activity>("activities", conditions);
}

QList<Activity> Activity::findCurrent()
{
  return find("WHERE activities.ended_at IS NULL");
}

QList<Activity> Activity::findToday()
{
  return findDay(QDate::currentDate());
}

QList<Activity> Activity::findDay(QDate date)
{
  return find(QString("WHERE date(activities.started_at) = date('%1')").arg(date.toString(Qt::ISODate)));
}

QMap<QString, int> Activity::projectTotals(QList<Activity> &activities)
{
  QMap<QString, int> totals;
  for (int i = 0; i < activities.size(); i++) {
    Activity &activity = activities[i];
    int total = (totals.contains(activity.projectName()) ? totals[activity.projectName()] : 0);
    total += activity.duration();
    totals.insert(activity.projectName(), total);
  }
  return totals;
}

QList<QString> Activity::distinctNames()
{
  QSqlDatabase &database = getDatabase();
  QSqlQuery query = database.exec(distinctNamesQuery);

  QList<QString> names;
  while (query.next()) {
    QString name = query.value(0).toString();
    QVariant projectName = query.value(1);
    if (projectName.isNull()) {
      names << name;
    }
    else {
      names << QString("%1@%2").arg(name).arg(projectName.toString());
    }
  }
  return names;
}

void Activity::stopCurrent()
{
  QSqlDatabase &database = getDatabase();
  database.exec(stopCurrentQuery);
  //qDebug() << deleteShortQuery;
  database.exec(deleteShortQuery);
}

QVariantList Activity::toVariantList(QList<Activity> &activities)
{
  QVariantList list;
  for (int i = 0; i < activities.size(); i++) {
    Activity activity = activities[i];
    list << activity.toVariantMap();
  }
  return list;
}

// Constructors
Activity::Activity(QObject *parent)
  : Model(parent)
{
  running = QVariant(QVariant::Bool);
}

Activity::Activity(QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent)
  : Model(attributes, newRecord, parent)
{
  running = QVariant(QVariant::Bool);
}

// Attribute getters/setters
QString Activity::name()
{
  return get("name").toString();
}

void Activity::setName(QString name)
{
  set("name", name);
}

int Activity::projectId()
{
  QVariant projectId = get("project_id");
  if (projectId.isNull() || !projectId.isValid()) {
    return -1;
  }
  return projectId.toInt();
}

void Activity::setProjectId(int projectId)
{
  if (projectId >= 0) {
    set("project_id", projectId);
  }
  else {
    unset("project_id");
  }
}

QDateTime Activity::startedAt()
{
  return get("started_at").toDateTime();
}

void Activity::setStartedAt(QDateTime startedAt)
{
  set("started_at", startedAt);
}

QDateTime Activity::endedAt()
{
  return get("ended_at").toDateTime();
}

void Activity::setEndedAt(QDateTime endedAt)
{
  set("ended_at", endedAt);
}

void Activity::setFromParams(const QList<QPair<QString, QString> > &params)
{
  for (int i = 0; i < params.size(); i++) {
    const QPair<QString, QString> pair = params[i];
    if (pair.first == "activity[name_with_project]") {
      setNameWithProject(pair.second);
    }
    else if (pair.first == "activity[running]") {
      setRunning(pair.second == "true");
    }
  }
}

// Non-attribute getters and setters
bool Activity::isRunning()
{
  if (isNew() || !running.isNull()) {
    return running.toBool();
  }
  else {
    return endedAt().isValid();
  }
}

void Activity::setRunning(bool running)
{
  this->running = QVariant(running);
}

QString Activity::nameWithProject()
{
  QStringList strings;
  QString pname = projectName();

  strings << name();
  if (!pname.isEmpty()) {
    strings << pname;
  }
  return strings.join("@");
}

void Activity::setNameWithProject(QString nameWithProject)
{
  // NOTE: this could create orphaned projects if the activity is not saved
  //       after this method is called
  QStringList parts = nameWithProject.split("@");
  setName(parts[0]);

  if (parts.size() == 2 && !parts[1].isEmpty()) {
    int projectId = Project::findOrCreateByName(parts[1]);
    setProjectId(projectId);
  }
  else {
    setProjectId(-1);
  }
}

// Helpers
Project Activity::project()
{
  int id = projectId();
  return id >= 0 ? Project::findById(id) : Project();
}

QString Activity::projectName()
{
  int id = projectId();
  return id >= 0 ? Project::findById(id).name() : QString();
}

QString Activity::tagNames()
{
  QList<Tag> tags = Tag::findActivityTags(id());
  QStringList names;
  for (int i = 0; i < tags.size(); i++) {
    names << tags[i].name();
  }
  return names.join(", ");
}

QString Activity::startedAtMDY()
{
  QDateTime date = startedAt();
  if (date.isValid()) {
    return date.toString("MM/dd/yyyy");
  }
  else {
    return QString();
  }
}

QString Activity::startedAtHM()
{
  QDateTime date = startedAt();
  if (date.isValid()) {
    return date.toString("hh:mm");
  }
  else {
    return QString();
  }
}

QString Activity::startedAtISO8601()
{
  QDateTime date = startedAt();
  if (date.isValid()) {
    return date.toUTC().toString(Qt::ISODate);
  }
  else {
    return QString();
  }
}

QString Activity::endedAtMDY()
{
  QDateTime date = endedAt();
  if (date.isValid()) {
    return date.toString("MM/dd/yyyy");
  }
  else {
    return QString();
  }
}

QString Activity::endedAtHM()
{
  QDateTime date = endedAt();
  if (date.isValid()) {
    return date.toString("hh:mm");
  }
  else {
    return QString();
  }
}

QString Activity::endedAtISO8601()
{
  QDateTime date = endedAt();
  if (date.isValid()) {
    return date.toUTC().toString(Qt::ISODate);
  }
  else {
    return QString();
  }
}

int Activity::duration()
{
  QDateTime start = startedAt();
  if (start.isValid()) {
    QDateTime end = endedAt();
    if (end.isValid()) {
      return start.secsTo(end);
    }
    else {
      return start.secsTo(QDateTime::currentDateTime());
    }
  }
  else {
    return -1;
  }
}

QString Activity::durationInWords()
{
  int d = duration();
  if (d >= 0) {
    int totalMinutes = d / 60;

    if (totalMinutes == 0) {
      return QString("0min");
    }
    else {
      int minutes = totalMinutes % 60;
      int totalHours = totalMinutes / 60;
      int hours = totalHours % 24;
      int days = totalHours / 24;
      //qDebug() << "minutes:" << minutes << "; hours:" << hours << "; days:" << days;

      QStringList strings;
      if (days > 0) {
        strings << QString("%1d").arg(days);
      }
      if (hours > 0) {
        strings << QString("%1h").arg(hours);
      }
      if (minutes > 0) {
        strings << QString("%1min").arg(minutes);
      }
      return strings.join(" ");
    }
  }
  else {
    return QString();
  }
}

QVariantMap Activity::toVariantMap()
{
  QVariantMap map;

  map["id"] = id();
  map["name"] = name();
  map["startedAtISO8601"] = startedAtISO8601();
  map["startedAtHM"] = startedAtHM();
  map["startedAtMDY"] = startedAtMDY();
  map["endedAtHM"] = endedAtHM();
  map["endedAtMDY"] = endedAtMDY();
  map["durationInWords"] = durationInWords();
  map["isRunning"] = isRunning() ? 0 : 1;
  map["projectName"] = projectName();
  map["nameWithProject"] = nameWithProject();
  map["tagNames"] = tagNames();

  return map;
}

// Overriden Model functions
bool Activity::save()
{
  return Model::save("activities");
}

bool Activity::validate()
{
  if (name().isEmpty()) {
    qDebug() << "name was empty";
    return false;
  }
  if (!startedAt().isValid()) {
    qDebug() << "startedAt was empty";
    return false;
  }
  if (endedAt().isValid() && duration() < 0) {
    qDebug() << "endedAt was earlier than startedAt";
    return false;
  }
  if (!isRunning() && !endedAt().isValid()) {
    qDebug() << "endedAt was empty";
    return false;
  }

  return true;
}
