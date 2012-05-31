#include <QtDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include "activity.h"
#include "tag.h"

const QString Activity::findQuery = QString(
    "SELECT activities.id, activities.name, activities.project_id, "
    "activities.started_at, activities.ended_at, "
    "projects.name AS project_name "
    "FROM activities "
    "LEFT JOIN projects ON activities.project_id = projects.id");

const QString Activity::distinctNamesQuery = QString(
    "SELECT DISTINCT activities.name, projects.name AS project_name "
    "FROM activities "
    "LEFT JOIN projects ON activities.project_id = projects.id "
    "ORDER BY activities.name, projects.name");

const QString Activity::insertQuery = QString(
    "INSERT INTO activities (name, project_id, started_at, ended_at) "
    "VALUES(?, ?, ?, ?)");

QList<Activity> Activity::find(QString conditions)
{
  QStringList queryStrings;
  queryStrings << findQuery;
  if (!conditions.isEmpty()) {
    queryStrings << conditions;
  }
  qDebug() << queryStrings.join(" ");

  QSqlDatabase &database = getDatabase();
  QSqlQuery query = database.exec(queryStrings.join(" "));

  QList<Activity> result;
  while (query.next()) {
    QMap<QString, QVariant> attributes;
    attributes["id"] = query.value(0);
    attributes["name"] = query.value(1);
    attributes["project_id"] = query.value(2);
    attributes["started_at"] = query.value(3);
    attributes["ended_at"] = query.value(4);
    attributes["project_name"] = query.value(5);

    Activity activity(attributes);
    result << activity;
  }
  return result;
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

bool Activity::createFromParams(const QList<QPair<QString, QString> > &params)
{
  QSqlDatabase &database = getDatabase();
  QSqlQuery query(database);
  query.prepare(insertQuery);

  bool nameWithProjectFound = false;
  for (int i = 0; i < params.size(); i++) {
    const QPair<QString, QString> pair = params[i];
    if (pair.first == "activity[name_with_project]") {
      nameWithProjectFound = true;
      if (pair.first.isEmpty()) {
        return false;
      }

      QStringList parts = pair.second.split("@");
      if (parts[0].isEmpty()) {
        return false;
      }
      query.bindValue(0, QVariant(parts[0]));

      if (parts.size() == 2 && !parts[1].isEmpty()) {
        int projectId = Project::findOrCreateByName(parts[1]);
        query.bindValue(1, QVariant(projectId));
      }
      else {
        query.bindValue(1, QVariant(QVariant::Int));
      }
    }
  }

  if (nameWithProjectFound) {
    query.bindValue(2, QVariant(QDateTime::currentDateTime()));
    query.bindValue(3, QVariant(QVariant::DateTime));
    return query.exec();
  }
  else {
    return false;
  }
}

Activity::Activity(QObject *parent)
  : Model(parent)
{
}

Activity::Activity(QMap<QString, QVariant> &attributes, QObject *parent)
  : Model(attributes, parent)
{
}

QString Activity::name()
{
  return get("name").toString();
}

int Activity::projectId()
{
  QVariant projectId = get("project_id");
  if (projectId.isNull() || !projectId.isValid()) {
    return -1;
  }
  return projectId.toInt();
}

QDateTime Activity::startedAt()
{
  return get("started_at").toDateTime();
}

QDateTime Activity::endedAt()
{
  return get("ended_at").toDateTime();
}

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
    return date.toString(Qt::ISODate);
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
    return date.toString(Qt::ISODate);
  }
  else {
    return QString();
  }
}

int Activity::duration()
{
  QDateTime start = startedAt();
  QDateTime end = endedAt();
  if (end.isValid()) {
    return start.secsTo(end);
  }
  else {
    return start.secsTo(QDateTime::currentDateTime());
  }
}

QString Activity::durationInWords()
{
  int d = duration();
  int totalMinutes = d / 60;

  if (totalMinutes == 0) {
    return QString("0min");
  }
  else {
    int minutes = totalMinutes % 60;
    int totalHours = totalMinutes / 60;
    int hours = totalHours % 24;
    int days = totalHours / 24;
    qDebug() << "minutes:" << minutes << "; hours:" << hours << "; days:" << days;

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

bool Activity::isRunning()
{
  return endedAt().isValid();
}
