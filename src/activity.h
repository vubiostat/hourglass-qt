#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <QSqlDatabase>
#include <QString>
#include <QDateTime>
#include <QList>

class Activity : public QObject
{
  Q_OBJECT

  public:
    static QList<Activity> findAll();
    static QSqlDatabase getDatabase();

    Activity(QString name, int projectId, QDateTime startedAt, QDateTime endedAt);
    Activity(const Activity &other);
    Activity &operator=(const Activity &other);

    int id;
    QString name;
    int projectId;
    QDateTime startedAt;
    QDateTime endedAt;
};

#endif
