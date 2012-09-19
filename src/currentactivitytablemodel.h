#ifndef __CURRENTACTIVITYTABLEMODEL_H
#define __CURRENTACTIVITYTABLEMODEL_H

#include "activitytablemodel.h"

class CurrentActivityTableModel : public ActivityTableModel
{
  Q_OBJECT

  public:
    CurrentActivityTableModel(RecordManager<Activity> *recordManager, QObject *parent = 0);
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

  protected:
    //bool activityChangesSince(const QDateTime &dateTime) const;
    QList<int> fetchActivityIds() const;
    bool containsActivity(QSharedPointer<Activity> ptr) const;
};

#endif
