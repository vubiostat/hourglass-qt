#include "hourglass/activitynameslistmodel.h"
#include "hourglass/activity.h"

ActivityNamesListModel::ActivityNamesListModel(QObject *parent)
  : QAbstractListModel(parent)
{
  m_names = fetchNames();
}

int ActivityNamesListModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_names.count();
}

QVariant ActivityNamesListModel::data(const QModelIndex &index, int role) const
{
  switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
      return m_names.at(index.row());

    default:
      break;
  }
  return QVariant();
}

void ActivityNamesListModel::refreshNames()
{
  beginResetModel();
  m_names = fetchNames();
  endResetModel();
}

QStringList ActivityNamesListModel::fetchNames() const
{
  return Activity::distinctNames();
}
