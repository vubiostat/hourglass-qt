#include "currentactivitytablemodel.h"
#include <QFont>

CurrentActivityTableModel::CurrentActivityTableModel(RecordManager<Activity> *recordManager, QObject *parent)
  : ActivityTableModel(recordManager, parent)
{
}

int CurrentActivityTableModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 5;
}

QVariant CurrentActivityTableModel::data(const QModelIndex &index, int role) const
{
  QSharedPointer<Activity> activity = activityAt(index.row());
  switch (role) {
    case Qt::DisplayRole:
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
      break;

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

  return QVariant();
}

/*
bool CurrentActivityTableModel::activityChangesSince(const QDateTime &dateTime) const
{
  if (dateTime.isValid()) {
    qDebug() << "Checking for changes for current activities";

    QStringList activityIds;
    for (int i = 0; i < rowCount(); i++) {
      activityIds.append(QString::number(activityAt(i)->id()));
    }
    return Activity::countRunningChangesSince(dateTime, activityIds) > 0;
  }
  else {
    return true;
  }
}
*/

QList<int> CurrentActivityTableModel::fetchActivityIds() const
{
  return Activity::findCurrentIds();
}

bool CurrentActivityTableModel::containsActivity(QSharedPointer<Activity> activity) const
{
  if (activity->isRunning()) {
    qDebug() << "Activity" << activity->id() << "belongs to model for current activities";
    return true;
  }
  else {
    qDebug() << "Activity" << activity->id() << "doesn't belong to model for current activities";
    return false;
  }
}
