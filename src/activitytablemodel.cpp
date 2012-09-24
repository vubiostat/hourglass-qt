#include "activitytablemodel.h"
#include <QIcon>

const QString ActivityTableModel::s_timeSeparator = QString("-");

ActivityTableModel::ActivityTableModel(RecordManager<Activity> *recordManager, QObject *parent)
  : AbstractActivityModel(recordManager, parent)
{
}

ActivityTableModel::ActivityTableModel(const QDate &date, RecordManager<Activity> *recordManager, QObject *parent)
  : AbstractActivityModel(date, recordManager, parent)
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

int ActivityTableModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 9;
}

QVariant ActivityTableModel::data(const QModelIndex &index, int role) const
{
  Activity *activity = static_cast<Activity *>(index.internalPointer());
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

void ActivityTableModel::connectActivity(Activity *activity)
{
  AbstractActivityModel::connectActivity(activity);
  connect(activity, SIGNAL(durationChanged()),
      this, SLOT(durationChanged()));
}

void ActivityTableModel::durationChanged()
{
  Activity *activity = static_cast<Activity *>(QObject::sender());

  int row = indexOfActivityId(activity->id());
  if (row >= 0) {
    QModelIndex modelIndex = index(row, 6);
    emit dataChanged(modelIndex, modelIndex);
  }
}
