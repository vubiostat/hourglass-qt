#include "activitytablemodel.h"

const QString ActivityTableModel::s_timeSeparator = QString("-");

const QIcon &ActivityTableModel::editIcon()
{
  static QIcon s_editIcon = QIcon();

  if (s_editIcon.isNull()) {
    s_editIcon.addFile(":/icons/accessories-text-editor-16x16.png", QSize(16, 16));
    s_editIcon.addFile(":/icons/accessories-text-editor-22x22.png", QSize(22, 22));
    s_editIcon.addFile(":/icons/accessories-text-editor-24x24.png", QSize(24, 24));
    s_editIcon.addFile(":/icons/accessories-text-editor-32x32.png", QSize(32, 32));
    s_editIcon.addFile(":/icons/accessories-text-editor-48x48.png", QSize(48, 48));
  }
  return s_editIcon;
}

const QIcon &ActivityTableModel::trashIcon()
{
  static QIcon s_trashIcon = QIcon();

  if (s_trashIcon.isNull()) {
    s_trashIcon.addFile(":/icons/user-trash-22x22.png", QSize(22, 22));
    s_trashIcon.addFile(":/icons/user-trash-24x24.png", QSize(24, 24));
    s_trashIcon.addFile(":/icons/user-trash-32x32.png", QSize(32, 32));
    s_trashIcon.addFile(":/icons/user-trash-48x48.png", QSize(48, 48));
  }
  return s_trashIcon;
}

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
          return editIcon();

        case 8:
          return trashIcon();
      }
      break;

    case Qt::TextAlignmentRole:
      switch (index.column()) {
        case 0:
        case 6:
          return (int) (Qt::AlignVCenter | Qt::AlignRight);
        case 2:
        case 3:
        case 4:
        case 5:
          return (int) (Qt::AlignVCenter | Qt::AlignLeft);
        default:
          return Qt::AlignCenter;
      }
      break;

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
