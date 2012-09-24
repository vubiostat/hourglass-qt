#ifndef __PROJECTTOTALSLISTMODEL_H
#define __PROJECTTOTALSLISTMODEL_H

#include "abstractactivitymodel.h"

class ProjectTotalsListModel : public AbstractActivityModel
{
  Q_OBJECT

  public:
    ProjectTotalsListModel(const QDate &date, RecordManager<Activity> *recordManager, QObject *parent = 0);

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

  protected:
    void afterRefresh();

  private:
    QHash<QString, int> m_projectTotals;
    QStringList m_projectNames;
};

#endif
