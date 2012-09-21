#include "activitytablemodel.h"
#include <QIcon>
#include <QtDebug>

const QString ActivityTableModel::s_timeSeparator = QString("-");

ActivityTableModel::ActivityTableModel(RecordManager<Activity> *recordManager, QObject *parent)
  : QAbstractTableModel(parent), m_recordManager(recordManager)
{
}

ActivityTableModel::ActivityTableModel(QDate date, RecordManager<Activity> *recordManager, QObject *parent)
  : QAbstractTableModel(parent), m_recordManager(recordManager), m_date(date)
{
}

Qt::ItemFlags ActivityTableModel::flags(const QModelIndex &index) const
{
  if (index.column() < 6) {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  }
  else {
    return Qt::ItemIsEnabled;
  }
}

int ActivityTableModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return activityCount();
}

int ActivityTableModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 9;
}

QVariant ActivityTableModel::data(const QModelIndex &index, int role) const
{
  QSharedPointer<Activity> activity = activityAt(index.row());
  switch (role) {
    case Qt::DisplayRole:
      switch (index.column()) {
        case 0:
          if (activity->isUntimed()) {
            return QVariant();
          }
          return activity->startedAtHM();

        case 1:
          if (activity->isUntimed()) {
            return QVariant();
          }
          return s_timeSeparator;

        case 2:
          if (activity->isUntimed()) {
            return QVariant();
          }
          return activity->endedAtHM();

        case 3:
          return activity->name();

        case 4:
          return activity->projectName();

        case 5:
          /* Tags */
          return activity->tagNames();

        case 6:
          return activity->durationInWords();
      }
      break;

    case Qt::DecorationRole:
      switch (index.column()) {
        case 7:
          return QIcon::fromTheme("accessories-text-editor");

        case 8:
          return QIcon::fromTheme("user-trash");
      }
      break;

    case Qt::TextAlignmentRole:
      if (index.column() >= 3 && index.column() <= 5) {
        return (int) (Qt::AlignVCenter | Qt::AlignLeft);
      }
      else if (index.column() == 6) {
        return (int) (Qt::AlignVCenter | Qt::AlignRight);
      }
      else {
        return Qt::AlignCenter;
      }

    case Qt::ForegroundRole:
      if (index.column() == 4) {
        return Qt::gray;
      }
      break;

    default:
      break;
  }
  return QVariant();
}

QSharedPointer<Activity> ActivityTableModel::activityAt(int index) const
{
  return m_activities[m_activityIds[index]];
}

void ActivityTableModel::refreshActivities()
{
  beginResetModel();

  QList<int> previousActivityIds = m_activityIds;
  m_activityIds = fetchActivityIds();
  for (int i = 0; i < m_activityIds.count(); i++) {
    int activityId = m_activityIds[i];
    if (!previousActivityIds.removeOne(activityId)) {
      QSharedPointer<Activity> ptr = m_recordManager->getRecordById(activityId);

      /* Connect new activity */
      connect(ptr.data(), SIGNAL(durationChanged()),
          this, SLOT(activityDurationChanged()));
      connect(ptr.data(), SIGNAL(destroyed()),
          this, SLOT(internalActivityDestroyed()));
      connect(ptr.data(), SIGNAL(saved()),
          this, SLOT(internalActivitySaved()));
      connect(ptr.data(), SIGNAL(started()),
          this, SIGNAL(activityStarted()));

      m_activities[activityId] = ptr;
      qDebug() << "Activity" << activityId << "added.";
    }
  }

  /* Remove old activities */
  for (int i = 0; i < previousActivityIds.count(); i++) {
    int activityId = previousActivityIds[i];
    QSharedPointer<Activity> ptr = m_activities.take(activityId);
    disconnect(ptr.data(), 0, this, 0);
  }

  m_lastFetchedAt = QDateTime::currentDateTime();
  afterRefresh();
  endResetModel();
}

void ActivityTableModel::activityCreated(QSharedPointer<Activity> activity)
{
  if (containsActivity(activity)) {
    refreshActivities();
  }
}

void ActivityTableModel::internalActivityDestroyed()
{
  qDebug() << "An activity was destroyed!";
  refreshActivities();
  emit activityDestroyed();
}

void ActivityTableModel::internalActivitySaved()
{
  qDebug() << "An activity was saved!";
  refreshActivities();
  emit activitySaved();
}

/*
bool ActivityTableModel::activityChangesSince(const QDateTime &dateTime) const
{
  int count = 0;

  if (dateTime.isValid()) {
    if (m_date.isValid()) {
      qDebug() << "Checking for changes for activities for day of week" << m_date.dayOfWeek();
      count = Activity::countChangesSince(m_date, dateTime);
    }
    else {
      count = Activity::countChangesSince(dateTime);
    }
    return count > 0;
  }
  else {
    return true;
  }
}
*/

QList<int> ActivityTableModel::fetchActivityIds() const
{
  if (m_date.isValid()) {
    return Activity::findDayIds(m_date);
  }
  else {
    return Activity::findIds();
  }
}

bool ActivityTableModel::containsActivity(QSharedPointer<Activity> activity) const
{
  bool result = true;
  if (m_date.isValid()) {
    result = (activity->isUntimed() && activity->day() == m_date) ||
        (!activity->isUntimed() && activity->startedAt().date() == m_date);

    if (result) {
      qDebug() << "Activity" << activity->id() << "belongs to model for day of week" << m_date.dayOfWeek();
    }
    else {
      qDebug() << "Activity" << activity->id() << "doesn't belong to model for day of week" << m_date.dayOfWeek();
    }
  }

  return result;
}

void ActivityTableModel::afterRefresh()
{
}

int ActivityTableModel::activityCount() const
{
  return m_activityIds.count();
}

void ActivityTableModel::activityDurationChanged()
{
  Activity *activity = static_cast<Activity *>(QObject::sender());

  int index = m_activityIds.indexOf(activity->id());
  if (index >= 0) {
    QModelIndex modelIndex = createIndex(index, 6);
    emit dataChanged(modelIndex, modelIndex);
  }
}
