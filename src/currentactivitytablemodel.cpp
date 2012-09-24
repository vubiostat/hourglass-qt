#include "currentactivitytablemodel.h"
#include <QFont>

CurrentActivityTableModel::CurrentActivityTableModel(RecordManager<Activity> *recordManager, QObject *parent)
  : AbstractActivityModel(recordManager, parent), m_empty(true)
{
}

int CurrentActivityTableModel::rowCount(const QModelIndex &parent) const
{
  if (m_empty) {
    return 1;
  }
  else {
    return AbstractActivityModel::rowCount(parent);
  }
}

int CurrentActivityTableModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 5;
}

QVariant CurrentActivityTableModel::data(const QModelIndex &index, int role) const
{
  if (role == Qt::DisplayRole) {
    if (m_empty) {
      switch (index.column()) {
        case 0:
          return "No activity";

        case 4:
          return -1;
      }
    }
    else {
      Activity *activity = static_cast<Activity *>(index.internalPointer());
      switch (index.column()) {
        case 0:
          return activity->name();

        case 1:
          return activity->projectName();

        case 2:
          return activity->tagNames();

        case 3:
          return activity->durationInWords();

        case 4:
          return activity->id();
      }
    }
  }
  else {
    switch (role) {
      case Qt::FontRole:
        if (index.column() == 0) {
          QFont font;
          font.setPointSize(font.pointSize() + 6);
          font.setBold(true);
          return font;
        }
        break;

      case Qt::ForegroundRole:
        if (index.column() == 1) {
          return Qt::darkGray;
        }
        break;

      default:
        break;
    }
  }

  return QVariant();
}

bool CurrentActivityTableModel::isEmpty() const
{
  return m_empty;
}

QList<int> CurrentActivityTableModel::fetchActivityIds() const
{
  return Activity::findCurrentIds();
}

bool CurrentActivityTableModel::containsActivity(QSharedPointer<Activity> activity) const
{
  return activity->isRunning();
}

void CurrentActivityTableModel::afterRefresh()
{
  m_empty = activityCount() == 0;
}
