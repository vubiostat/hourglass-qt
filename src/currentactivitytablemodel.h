#ifndef __CURRENTACTIVITYTABLEMODEL_H
#define __CURRENTACTIVITYTABLEMODEL_H

#include "abstractactivitymodel.h"

class CurrentActivityTableModel : public AbstractActivityModel
{
  Q_OBJECT

  public:
    CurrentActivityTableModel(RecordManager<Activity> *recordManager, QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool isEmpty() const;

  protected:
    QList<int> fetchActivityIds() const;
    bool containsActivity(QSharedPointer<Activity> ptr) const;
    void afterRefresh();

  private:
    bool m_empty;
};

#endif
