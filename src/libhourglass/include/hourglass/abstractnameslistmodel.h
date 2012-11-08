#ifndef __ABSTRACTNAMESLISTMODEL_H
#define __ABSTRACTNAMESLISTMODEL_H

#include <QAbstractListModel>
#include <QMap>
#include <QSharedPointer>
#include "activity.h"

class AbstractNamesListModel : public QAbstractListModel
{
  Q_OBJECT

  public:
    AbstractNamesListModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

  public slots:
    void refreshNames();
    virtual void activityCreated(QSharedPointer<Activity> activity) = 0;
    virtual void activitySaved(QSharedPointer<Activity> activity) = 0;
    virtual void activityDestroyed(QSharedPointer<Activity> activity) = 0;

  protected:
    virtual QMap<QString, int> fetchNames() const = 0;
    void removeName(const QString &name);
    void addName(const QString &name);

  private:
    QMap<QString, int> m_names;

};

#endif
