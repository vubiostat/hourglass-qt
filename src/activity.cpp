#include <QSqlQuery>
#include <QVariant>
#include "database.h"
#include "activity.h"

QList<Activity> Activity::findAll()
{
  QSqlDatabase database = getDatabase();
  QSqlQuery query = database.exec("SELECT id, name, project_id, started_at, ended_at FROM activities");

  QList<Activity> result;
  while (query.next()) {
    int id        = query.value(0).toInt();
    QString name  = query.value(1).toString();
    int projectId = query.value(2).toInt();
    QDateTime startedAt = query.value(3).toDateTime();
    QDateTime endedAt   = query.value(4).toDateTime();

    Activity activity(name, projectId, startedAt, endedAt);
    activity.id = id;
    result << activity;
  }
  return result;
}

QSqlDatabase Activity::getDatabase()
{
  return DatabaseManager::getInstance().getDatabase();
}

Activity::Activity(QString name, int projectId, QDateTime startedAt, QDateTime endedAt)
{
  this->name = name;
  this->projectId = projectId;
  this->startedAt = startedAt;
  this->endedAt = endedAt;
}

Activity::Activity(const Activity &other)
{
  this->id = other.id;
  this->name = other.name;
  this->projectId = other.projectId;
  this->startedAt = other.startedAt;
  this->endedAt = other.endedAt;
}

Activity &Activity::operator=(const Activity &other)
{
  this->id = other.id;
  this->name = other.name;
  this->projectId = other.projectId;
  this->startedAt = other.startedAt;
  this->endedAt = other.endedAt;
}
