#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <QSqlDatabase>
#include <QString>
#include <QDateTime>
#include <QDate>
#include <QList>
#include <QPair>
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
    static bool createFromParams(const QList<QPair<QString, QString> > &params);
    static void stopCurrent();

    Activity(QObject *parent = 0);
    Activity(QMap<QString, QVariant> &attributes, QObject *parent = 0);

    QString name();
    int projectId();
    QDateTime startedAt();
    QDateTime endedAt();
    Project project();
    QString projectName();
    QString tagNames();

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
    static const QString distinctNamesQuery;
    static const QString insertQuery;
    static const QString stopCurrentQuery;
    static const QString deleteShortQuery;
};

#endif
