#include "abstractactivitymodel.h"

AbstractActivityModel::AbstractActivityModel(RecordManager<Activity> *recordManager, QObject *parent)
  : QAbstractItemModel(parent), m_recordManager(recordManager), m_allUntimed(true)
{
}

AbstractActivityModel::AbstractActivityModel(const QDate &date, RecordManager<Activity> *recordManager, QObject *parent)
  : QAbstractItemModel(parent), m_date(date), m_recordManager(recordManager), m_allUntimed(true)
{
}

QModelIndex AbstractActivityModel::index(int row, int column, const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return createIndex(row, column);
}

QModelIndex AbstractActivityModel::parent(const QModelIndex &index) const
{
  Q_UNUSED(index);

  return QModelIndex();
}

int AbstractActivityModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return activityCount();
}

QSharedPointer<Activity> AbstractActivityModel::activityAt(int index) const
{
  return m_activities[m_activityIds[index]];
}

bool AbstractActivityModel::allUntimed() const
{
  return m_allUntimed;
}

void AbstractActivityModel::refreshActivities()
{
  beginResetModel();

  m_allUntimed = true;

  QList<int> previousActivityIds = m_activityIds;
  m_activityIds = fetchActivityIds();
  for (int i = 0; i < m_activityIds.count(); i++) {
    int activityId = m_activityIds[i];
    if (!previousActivityIds.removeOne(activityId)) {
      QSharedPointer<Activity> ptr = m_recordManager->getRecordById(activityId);
      connectActivity(ptr.data());

      m_activities[activityId] = ptr;
    }
    if (!m_activities[activityId]->isUntimed()) {
      m_allUntimed = false;
    }
  }

  /* Remove old activities */
  for (int i = 0; i < previousActivityIds.count(); i++) {
    int activityId = previousActivityIds[i];
    QSharedPointer<Activity> ptr = m_activities.take(activityId);
    disconnect(ptr.data(), 0, this, 0);
  }

  afterRefresh();
  endResetModel();
}

bool AbstractActivityModel::containsActivity(QSharedPointer<Activity> activity) const
{
  bool result = true;
  if (m_date.isValid()) {
    result = (activity->isUntimed() && activity->day() == m_date) ||
        (!activity->isUntimed() && activity->startedAt().date() == m_date);
  }
  return result;
}

QList<int> AbstractActivityModel::fetchActivityIds() const
{
  const QDate &d = date();
  if (d.isValid()) {
    return Activity::findDayIds(d);
  }
  else {
    return Activity::findIds();
  }
}

void AbstractActivityModel::connectActivity(Activity *activity)
{
  connect(activity, SIGNAL(destroyed()),
      this, SLOT(destroyed()));
  connect(activity, SIGNAL(saved()),
      this, SLOT(saved()));
  connect(activity, SIGNAL(started()),
      this, SIGNAL(activityStarted()));
}

void AbstractActivityModel::afterRefresh()
{
}

int AbstractActivityModel::activityCount() const
{
  return m_activityIds.count();
}

int AbstractActivityModel::indexOfActivityId(int activityId) const
{
  return m_activityIds.indexOf(activityId);
}

const QDate &AbstractActivityModel::date() const
{
  return m_date;
}

void AbstractActivityModel::created(QSharedPointer<Activity> activity)
{
  if (containsActivity(activity)) {
    refreshActivities();
  }
}

void AbstractActivityModel::destroyed()
{
  refreshActivities();
  emit activityDestroyed();
}

void AbstractActivityModel::saved()
{
  refreshActivities();
  emit activitySaved();
}
