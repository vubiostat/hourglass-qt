#include "activitytablemodel.h"
#include <QIcon>
#include <QtDebug>

const QString ActivityTableModel::s_timeSeparator = QString("-");

ActivityTableModel::ActivityTableModel(QObject *parent)
  : QAbstractTableModel(parent)
{
  fetchActivities();
}

ActivityTableModel::ActivityTableModel(QDate date, QObject *parent)
  : QAbstractTableModel(parent), m_date(date)
{
  fetchActivities();
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
  return m_activities.count();
}

int ActivityTableModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 9;
}

QVariant ActivityTableModel::data(const QModelIndex &index, int role) const
{
  Activity *activity = m_activities.at(index.row());
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
      return QVariant();
  }
  return QVariant();
}

void ActivityTableModel::fetchActivities()
{
  int num;

  if (m_lastFetchedAt.isValid()) {
    if (m_date.isValid()) {
      num = Activity::countChangesSince(m_date, m_lastFetchedAt);
    }
    else {
      num = Activity::countChangesSince(m_lastFetchedAt);
    }
  }
  else {
    num = 1;
  }

  if (num > 0) {
    beginResetModel();

    /* Disconnect existing activities */
    for (int i = 0; i < m_activities.count(); i++) {
      Activity *activity = m_activities[i];
      disconnect(activity, SIGNAL(durationChanged()),
          this, SLOT(activityDurationChanged()));
      activity->deleteLater();
    }

    if (m_date.isValid()) {
      m_activities = Activity::findDay(m_date, this);
    }
    else {
      m_activities = Activity::find(this);
    }
    m_lastFetchedAt = QDateTime::currentDateTime();

    /* Connect new activities */
    for (int i = 0; i < m_activities.count(); i++) {
      Activity *activity = m_activities[i];
      connect(activity, SIGNAL(durationChanged()),
          this, SLOT(activityDurationChanged()));
    }

    endResetModel();
  }
}

void ActivityTableModel::activityDurationChanged()
{
  Activity *activity = static_cast<Activity *>(QObject::sender());

  int index = m_activities.indexOf(activity);
  if (index >= 0) {
    QModelIndex modelIndex = createIndex(index, 6);
    emit dataChanged(modelIndex, modelIndex);
  }
}
