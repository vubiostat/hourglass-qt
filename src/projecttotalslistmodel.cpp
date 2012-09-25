#include "projecttotalslistmodel.h"

ProjectTotalsListModel::ProjectTotalsListModel(const QDate &date, RecordManager<Activity> *recordManager, QObject *parent)
  : AbstractActivityModel(date, recordManager, parent)
{
}

int ProjectTotalsListModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 1;
}

int ProjectTotalsListModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return m_projectTotals.count();
}

QVariant ProjectTotalsListModel::data(const QModelIndex &index, int role) const
{
  switch (role) {
    case Qt::DisplayRole:
      if (index.row() < m_projectTotals.count()) {
        QString projectName = m_projectNames[index.row()];
        int duration = m_projectTotals[projectName];
        return QString("%1: %2h").arg(projectName).
          arg(((double) duration) / ((double) 3600), 4, 'f', 2, '0');
      }
      break;

    default:
      break;
  }

  return QVariant();
}

void ProjectTotalsListModel::connectActivity(Activity *activity)
{
  AbstractActivityModel::connectActivity(activity);
  connect(activity, SIGNAL(durationChanged()),
      this, SLOT(durationChanged()));
}

void ProjectTotalsListModel::afterRefresh()
{
  m_projectNames.clear();
  m_projectTotals.clear();
  for (int i = 0; i < activityCount(); i++) {
    QSharedPointer<Activity> activity = activityAt(i);
    const QString &projectName = activity->projectDisplayName();
    if (m_projectTotals.contains(projectName)) {
      m_projectTotals[projectName] += activity->duration();
    }
    else {
      m_projectNames.append(projectName);
      m_projectTotals[projectName] = activity->duration();
    }
  }
}

void ProjectTotalsListModel::durationChanged()
{
  Activity *activity = static_cast<Activity *>(QObject::sender());
  const QString &projectName = activity->projectDisplayName();

  int row = m_projectNames.indexOf(projectName);
  if (row >= 0) {
    /* Update duration for this project */
    int newTotal = 0;
    for (int i = 0; i < activityCount(); i++) {
      QSharedPointer<Activity> activity = activityAt(i);
      if (projectName == activity->projectDisplayName()) {
        newTotal += activity->duration();
      }
    }
    m_projectTotals[projectName] = newTotal;

    QModelIndex modelIndex = index(row, 0);
    emit dataChanged(modelIndex, modelIndex);
  }
}
