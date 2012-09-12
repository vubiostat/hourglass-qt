#ifndef __ACTIVITY_H
#define __ACTIVITY_H

#include <QAbstractTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>
#include "database.h"

class ActivityTableModel : public QAbstractTableModel
{
  Q_OBJECT

  public:
    ActivityTableModel(QObject *parent = 0, QSqlDatabase database = QSqlDatabase::database());
    ActivityTableModel(QDate date, QObject *parent = 0, QSqlDatabase database = QSqlDatabase::database());
    ActivityTableModel(QDate startDate, QDate endDate, QObject *parent = 0, QSqlDatabase database = QSqlDatabase::database());

    Qt::ItemFlags flags(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

  private:
    static const QString s_queryString;
    static const QString s_dayQueryString;
    static const QString s_timeSeparator;

    QSqlDatabase m_database;
    QSqlQuery m_query;
    QList<QVariantHash> m_activities;

    void getActivities();
    QString formatDateTime(QVariant value) const;
    int duration(const QVariantHash &hash) const;
    QString durationInWords(int duration) const;
};

#endif
