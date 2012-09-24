#ifndef __ABSTRACTACTIVITYMODEL_H
#define __ABSTRACTACTIVITYMODEL_H

#include <QAbstractItemModel>
#include <QDateTime>
#include <QList>
#include <QSharedPointer>
#include "recordmanager.h"
#include "activity.h"

class AbstractActivityModel : public QAbstractItemModel
{
  Q_OBJECT

  public:
    AbstractActivityModel(RecordManager<Activity> *recordManager, QObject *parent = 0);
    AbstractActivityModel(const QDate &date, RecordManager<Activity> *recordManager, QObject *parent = 0);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QSharedPointer<Activity> activityAt(int index) const;

  public slots:
    void refreshActivities();

  signals:
    void activityStarted();
    void activitySaved();
    void activityDestroyed();

  protected:
    virtual bool containsActivity(QSharedPointer<Activity> ptr) const;
    virtual QList<int> fetchActivityIds() const;
    virtual void connectActivity(Activity *activity);
    virtual void afterRefresh();
    int activityCount() const;
    int indexOfActivityId(int activityId) const;
    const QDate &date() const;

  protected slots:
    void created(QSharedPointer<Activity> activity);
    void destroyed();
    void saved();

  private:
    QDate m_date;
    RecordManager<Activity> *m_recordManager;
    QList<int> m_activityIds;
    QHash<int, QSharedPointer<Activity> > m_activities;
};

#endif
