#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <QSqlDatabase>
#include <QString>
#include <QDateTime>
#include <QDate>
#include <QList>
#include <QMap>
#include "model.h"
#include "project.h"

class Activity : public Model
{
  Q_OBJECT

  public:
    static QList<Activity> find(QString conditions);
    static QList<Activity> findCurrent();
    static QList<Activity> findToday();
    static QList<Activity> findDay(QDate date);
    static QMap<QString, int> projectTotals(QList<Activity> &activities);
    static QList<QString> distinctNames();

    Activity(QObject *parent = 0);
    Activity(QMap<QString, QVariant> &attributes, QObject *parent = 0);

    int id();
    QString name();
    int projectId();
    QDateTime startedAt();
    QDateTime endedAt();
    Project project();
    QString projectName();

    QString startedAtMDY();
    QString startedAtHM();
    QString startedAtISO8601();
    QString endedAtMDY();
    QString endedAtHM();
    QString endedAtISO8601();
    int duration();
    QString durationInWords();
    bool isRunning();

  private:
    static const QString findQuery;
    static const QString tagsQuery;
    static const QString distinctNamesQuery;
};

#endif
