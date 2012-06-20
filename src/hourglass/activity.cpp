#include <QtDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QRegExp>
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

const QString Activity::addTagQuery = QString(
    "INSERT INTO activities_tags (activity_id, tag_id) "
    "VALUES(:activity_id, :tag_id)");

const QString Activity::removeTagQuery = QString(
    "DELETE FROM activities_tags WHERE "
    "activity_id = :activity_id AND tag_id = :tag_id");

QList<Activity> Activity::find(QString conditions)
{
  return Model::find<Activity>("activities", conditions, "ORDER BY started_at");
}

Activity Activity::findById(int id)
{
  return Model::findById<Activity>("activities", id);
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

bool Activity::startLike(const Activity &activity)
{
  QList<Activity> currentActivities = findCurrent();
  if (currentActivities.count() > 0) {
    Activity &current = currentActivities[0];
    if (activity.id() == current.id() || activity.isSimilarTo(current)) {
      return false;
    }
  }

  Activity result;
  result.setRunning(true);
  result.setNameWithProject(activity.nameWithProject());
  result.setStartedAt(QDateTime::currentDateTime());
  result.setTagNames(activity.tagNames());
  return result.save();
}

QDate Activity::dateFromMDY(const QString &mdy)
{
  QRegExp rx("^(\\d{1,2})/(\\d{1,2})/(\\d{4})$");
  if (mdy.contains(rx)) {
    return QDate(rx.cap(3).toInt(), rx.cap(1).toInt(), rx.cap(2).toInt());
  }
  else {
    return QDate();
  }
}

QTime Activity::timeFromHM(const QString &hm)
{
  QRegExp rx("^(\\d{1,2}):(\\d{2})$");
  if (hm.contains(rx)) {
    return QTime(rx.cap(1).toInt(), rx.cap(2).toInt());
  }
  else {
    return QTime();
  }
}

// Constructors
Activity::Activity(QObject *parent)
  : Model(parent)
{
  m_running = QVariant(QVariant::Bool);
  m_wasRunning = false;
}

Activity::Activity(QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent)
  : Model(attributes, newRecord, parent)
{
  m_running = QVariant(QVariant::Bool);
  m_wasRunning = isRunning();
}

// Attribute getters/setters
QString Activity::name() const
{
  return get("name").toString();
}

void Activity::setName(QString name)
{
  set("name", name);
}

int Activity::projectId() const
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

QDateTime Activity::startedAt() const
{
  return get("started_at").toDateTime();
}

void Activity::setStartedAt(QDateTime startedAt)
{
  set("started_at", startedAt);
}

QDateTime Activity::endedAt() const
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
    else if (pair.first == "activity[started_at_mdy]") {
      setStartedAtMDY(pair.second);
    }
    else if (pair.first == "activity[started_at_hm]") {
      setStartedAtHM(pair.second);
    }
    else if (pair.first == "activity[ended_at_mdy]") {
      setEndedAtMDY(pair.second);
    }
    else if (pair.first == "activity[ended_at_hm]") {
      setEndedAtHM(pair.second);
    }
    else if (pair.first == "activity[tag_names]") {
      setTagNames(pair.second);
    }
  }
}

// Non-attribute getters and setters
bool Activity::isRunning() const
{
  if (isNew() || !m_running.isNull()) {
    return m_running.toBool();
  }
  else {
    return !endedAt().isValid();
  }
}

void Activity::setRunning(bool running)
{
  this->m_running = QVariant(running);
  if (running) {
    setEndedAt(QDateTime());
  }
}

bool Activity::wasRunning() const
{
  return m_wasRunning.toBool();
}

QString Activity::nameWithProject() const
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

QString Activity::startedAtMDY() const
{
  QDateTime date = startedAt();
  if (date.isValid()) {
    return date.toString("MM/dd/yyyy");
  }
  else {
    return QString();
  }
}

void Activity::setStartedAtMDY(const QString &mdy)
{
  m_startedAtMDY = dateFromMDY(mdy);
}

QString Activity::startedAtHM() const
{
  QDateTime date = startedAt();
  if (date.isValid()) {
    return date.toString("hh:mm");
  }
  else {
    return QString();
  }
}

void Activity::setStartedAtHM(const QString &hm)
{
  m_startedAtHM = timeFromHM(hm);
}

QString Activity::endedAtMDY() const
{
  QDateTime date = endedAt();
  if (date.isValid()) {
    return date.toString("MM/dd/yyyy");
  }
  else {
    return QString();
  }
}

void Activity::setEndedAtMDY(const QString &mdy)
{
  m_endedAtMDY = dateFromMDY(mdy);
}

QString Activity::endedAtHM() const
{
  QDateTime date = endedAt();
  if (date.isValid()) {
    return date.toString("hh:mm");
  }
  else {
    return QString();
  }
}

void Activity::setEndedAtHM(const QString &hm)
{
  m_endedAtHM = timeFromHM(hm);
}

QString Activity::tagNames() const
{
  QList<Tag> currentTags = tags();
  QStringList names;
  for (int i = 0; i < currentTags.size(); i++) {
    names << currentTags[i].name();
  }
  return names.join(", ");
}

void Activity::setTagNames(const QString &tagNames)
{
  QList<Tag> previousTags = tags();
  QList<Tag> newTags;

  QString trimmedTagNames = tagNames.trimmed();
  bool tagExists;
  if (!trimmedTagNames.isEmpty()) {
    QStringList names = trimmedTagNames.split(QRegExp(",\\s*"));
    for (int i = 0; i < names.size(); i++) {
      Tag tag = Tag::findOrCreateByName(names[i]);
      tagExists = previousTags.removeOne(tag);
      if (!tagExists && !newTags.contains(tag)) {
        newTags << tag;
      }
    }
  }

  if (isNew()) {
    m_tagsToAdd = newTags;
  }
  else {
    addTags(newTags);
    removeTags(previousTags);
  }
}

// Helpers
Project Activity::project() const
{
  int id = projectId();
  return id >= 0 ? Project::findById(id) : Project();
}

QString Activity::projectName() const
{
  int id = projectId();
  return id >= 0 ? Project::findById(id).name() : QString();
}

QList<Tag> Activity::tags() const
{
  return Tag::findActivityTags(id());
}

QString Activity::startedAtISO8601() const
{
  QDateTime date = startedAt();
  if (date.isValid()) {
    return date.toUTC().toString(Qt::ISODate);
  }
  else {
    return QString();
  }
}

QString Activity::endedAtISO8601() const
{
  QDateTime date = endedAt();
  if (date.isValid()) {
    return date.toUTC().toString(Qt::ISODate);
  }
  else {
    return QString();
  }
}

int Activity::duration() const
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

QString Activity::durationInWords() const
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

QVariantMap Activity::toVariantMap() const
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

bool Activity::isSimilarTo(const Activity &other) const
{
  return other.nameWithProject() == nameWithProject() &&
    other.tagNames() == tagNames();
}

// Overriden Model functions
bool Activity::save()
{
  return Model::save("activities");
}

bool Activity::destroy()
{
  return Model::destroy("activities");
}

void Activity::beforeValidation()
{
  if (m_startedAtMDY.isValid() && m_startedAtHM.isValid()) {
    QDateTime date;
    date.setDate(m_startedAtMDY);
    date.setTime(m_startedAtHM);
    setStartedAt(date);
    m_startedAtMDY = QDate();
    m_startedAtHM = QTime();
  }

  if (m_endedAtMDY.isValid() && m_endedAtHM.isValid()) {
    QDateTime date;
    date.setDate(m_endedAtMDY);
    date.setTime(m_endedAtHM);
    setEndedAt(date);
    m_endedAtMDY = QDate();
    m_endedAtHM = QTime();
  }
}

void Activity::addTags(const QList<Tag> &tags)
{
  QSqlDatabase &database = getDatabase();
  QSqlQuery query(database);
  query.prepare(addTagQuery);

  for (int i = 0; i < tags.size(); i++) {
    query.bindValue(":activity_id", id());
    query.bindValue(":tag_id", tags[i].id());
    query.exec();
  }
}

void Activity::removeTags(const QList<Tag> &tags)
{
  QSqlDatabase &database = getDatabase();
  QSqlQuery query(database);
  query.prepare(removeTagQuery);

  QVariantList activityIds, tagIds;
  for (int i = 0; i < tags.size(); i++) {
    activityIds << id();
    tagIds << tags[i].id();
  }
  query.bindValue(":activity_id", activityIds);
  query.bindValue(":tag_id", tagIds);
  query.execBatch();
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

void Activity::beforeSave()
{
  if (isRunning() && !wasRunning()) {
    stopCurrent();
  }
}

void Activity::afterCreate()
{
  addTags(m_tagsToAdd);
  m_tagsToAdd.clear();
}
