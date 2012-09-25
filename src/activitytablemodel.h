#ifndef __ACTIVITYTABLEMODEL_H
#define __ACTIVITYTABLEMODEL_H

#include "abstractactivitymodel.h"
#include <QIcon>

class ActivityTableModel : public AbstractActivityModel
{
  Q_OBJECT

  public:
    ActivityTableModel(RecordManager<Activity> *recordManager, QObject *parent = 0);
    ActivityTableModel(const QDate &date, RecordManager<Activity> *recordManager, QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

  protected:
    void connectActivity(Activity *activity);

  private:
    static const QString s_timeSeparator;

    static const QIcon &editIcon();
    static const QIcon &trashIcon();

  private slots:
    void durationChanged();
};

#endif
