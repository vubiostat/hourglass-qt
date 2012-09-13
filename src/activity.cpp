#include <QtDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QRegExp>
#include <QSettings>
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
    "WHERE ended_at IS NULL AND untimed != 1");

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

QList<Activity> Activity::find()
{
  return find(QString(), "ORDER BY untimed, started_at, id");
}

QList<Activity> Activity::find(QString conditions)
{
  return find(conditions, "ORDER BY untimed, started_at, id");
}

QList<Activity> Activity::find(QString conditions, QString predicate)
{
  return Record::find<Activity>("activities", conditions, predicate);
}

Activity Activity::findById(int id)
{
  return Record::findById<Activity>("activities", id);
}

QList<Activity> Activity::findCurrent()
{
  return find("WHERE activities.ended_at IS NULL AND activities.untimed != 1");
}

QList<Activity> Activity::findToday()
{
  return findDay(QDate::currentDate());
}

QList<Activity> Activity::findDay(QDate date)
{
  return find(QString("WHERE date(activities.started_at) = date('%1') OR (activities.untimed = 1 AND date(activities.day) = date('%1'))").arg(date.toString(Qt::ISODate)));
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
  QSqlDatabase database = Activity::database();
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
  QSqlDatabase database = Activity::database();
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

// Determine the last reasonably-sized gap between activities
QPair<QDateTime, QDateTime> Activity::lastGap()
{
  QSettings settings;
  QDateTime now = QDateTime::currentDateTime();
  QDateTime dayStart(now.date(), settings.value("day_start", QTime(8, 0)).toTime());
  QDateTime lowerBound;
  int diff = dayStart.secsTo(now);
  if (diff >= 0 && diff < 14400) {
    lowerBound = dayStart;
  }
  else {
    // 4 hours ago
    lowerBound = now.addSecs(-14400);
  }
  QList<Activity> activities = find(
      QString("WHERE datetime(activities.started_at) >= datetime('%1') AND datetime(activities.started_at) <= datetime('%2')").
        arg(lowerBound.toString("yyyy-MM-dd hh:mm")).
        arg(now.toString("yyyy-MM-dd hh:mm")));

  QPair<QDateTime, QDateTime> gap;
  if (activities.empty()) {
    if (now < dayStart) {
      // It's earlier than the start of the day, so just go 15 minutes back
      gap.first = now.addSecs(-900);
      gap.second = now;
    }
    else {
      gap.first = dayStart;
      gap.second = dayStart.addSecs(900);
    }
    return gap;
  }

  gap.second = QDateTime::currentDateTime();
  for (int i = activities.size() - 1; i >= 0; i--) {
    if (!activities[i].isRunning()) {
      gap.first = activities[i].endedAt();
      if (gap.first.secsTo(gap.second) >= 900) {
        return gap;
      }
    }
    gap.second = activities[i].startedAt();
  }

  // No gap found, just pick the latest one possible
  Activity &lastActivity = activities.last();
  if (lastActivity.isRunning()) {
    gap.first = now.addSecs(-900);
  }
  else {
    gap.first = lastActivity.endedAt();
  }
  gap.second = now;
  return gap;
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
  : Record(parent)
{
  m_running = QVariant(QVariant::Bool);
  m_wasRunning = false;
}

Activity::Activity(QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent)
  : Record(attributes, newRecord, parent)
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

bool Activity::isUntimed() const
{
  int untimed = get("untimed").toInt();
  return untimed == 1;
}

void Activity::setUntimed(bool untimed)
{
  set("untimed", untimed ? 1 : 0);
}

int Activity::duration() const
{
  if (isUntimed()) {
    return get("duration").toInt();
  }
  else {
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
}

void Activity::setDuration(int duration)
{
  set("duration", duration);
}

QDate Activity::day() const
{
  if (isUntimed()) {
    return get("day").toDate();
  }
  else {
    QDateTime start = startedAt();
    if (start.isValid()) {
      return start.date();
    }
    else {
      return QDate();
    }
  }
}

void Activity::setDay(QDate day)
{
  set("day", day);
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
    else if (pair.first == "activity[untimed]") {
      bool ok = false;
      int value = pair.second.toInt(&ok);
      if (ok) {
        setUntimed(value != 0);
      }
    }
    else if (pair.first == "activity[duration]") {
      bool ok = false;
      int value = pair.second.toInt(&ok);
      if (ok) {
        setDuration(value);
      }
    }
    else if (pair.first == "activity[day_mdy]") {
      setDayMDY(pair.second);
    }
  }
}

// Non-attribute getters and setters
bool Activity::isRunning() const
{
  if (isUntimed()) {
    return false;
  }
  else if (isNew() || !m_running.isNull()) {
    return m_running.toBool();
  }
  else {
    return !endedAt().isValid();
  }
}

void Activity::setRunning(bool running)
{
  this->m_running = QVariant(running);
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

QString Activity::dayMDY() const
{
  QDate date = day();
  if (date.isValid()) {
    return date.toString("MM/dd/yyyy");
  }
  else {
    return QString();
  }
}

void Activity::setDayMDY(const QString &mdy)
{
  QDate date = dateFromMDY(mdy);
  if (date.isValid()) {
    setDay(date);
  }
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

bool Activity::occursOn(const QDate &date) const
{
  return (isUntimed() && day() == date) || (!isUntimed() && startedAt().date() == date);
}

// Overriden Record functions
bool Activity::save()
{
  return Record::save("activities");
}

bool Activity::destroy()
{
  return Record::destroy("activities");
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

  if (isRunning()) {
    setEndedAt(QDateTime());
  }
  else if (m_endedAtMDY.isValid() && m_endedAtHM.isValid()) {
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
  QSqlDatabase database = Activity::database();
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
  QSqlDatabase database = Activity::database();
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

  if (isUntimed()) {
    if (duration() <= 0) {
      qDebug() << "duration was less than or equal to 0";
      return false;
    }
    if (!day().isValid()) {
      qDebug() << "day was empty";
      return false;
    }
  }
  else {
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
  }

  return true;
}

void Activity::beforeSave()
{
  if (isRunning() && !wasRunning()) {
    stopCurrent();
  }
  if (isUntimed()) {
    unset("started_at");
    unset("ended_at");
  }
  else {
    unset("duration");
    unset("day");
  }
}

void Activity::afterCreate()
{
  addTags(m_tagsToAdd);
  m_tagsToAdd.clear();
}
