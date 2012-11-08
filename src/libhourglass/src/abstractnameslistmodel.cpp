#include "hourglass/abstractnameslistmodel.h"

AbstractNamesListModel::AbstractNamesListModel(QObject *parent)
  : QAbstractListModel(parent)
{
}

int AbstractNamesListModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_names.count();
}

QVariant AbstractNamesListModel::data(const QModelIndex &index, int role) const
{
  switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
      {
        QStringList keys = m_names.keys();
        return keys.at(index.row());
      }

    default:
      break;
  }
  return QVariant();
}

void AbstractNamesListModel::refreshNames()
{
  beginResetModel();
  m_names = fetchNames();
  endResetModel();
}

void AbstractNamesListModel::removeName(const QString &name)
{
  if (!m_names.contains(name)) {
    return;
  }
  m_names[name]--;

  if (m_names[name] <= 0) {
    QStringList keys = m_names.keys();
    int row = keys.indexOf(name);
    if (row >= 0) {
      beginRemoveRows(QModelIndex(), row, row);
      m_names.remove(name);
      endRemoveRows();
    }
  }
}

void AbstractNamesListModel::addName(const QString &name)
{
  if (m_names.contains(name)) {
    m_names[name]++;
    return;
  }

  QStringList keys = m_names.keys();
  int i;
  for (i = 0; i < keys.count(); i++) {
    if (keys[i].compare(name) > 0) {
      break;
    }
  }
  beginInsertRows(QModelIndex(), i, i);
  m_names[name] = 1;
  endInsertRows();
}
