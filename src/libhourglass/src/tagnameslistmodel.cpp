#include "hourglass/tagnameslistmodel.h"
#include "hourglass/tag.h"

TagNamesListModel::TagNamesListModel(QObject *parent)
  : QAbstractListModel(parent)
{
  m_names = fetchNames();
}

int TagNamesListModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_names.count();
}

QVariant TagNamesListModel::data(const QModelIndex &index, int role) const
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

void TagNamesListModel::refreshNames()
{
  beginResetModel();
  m_names = fetchNames();
  endResetModel();
}

QStringList TagNamesListModel::fetchNames() const
{
  return Tag::distinctNames();
}
