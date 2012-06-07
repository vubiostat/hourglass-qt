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
    static void stopCurrent();
    static QVariantList toVariantList(QList<Activity> &activities);

    Activity(QObject *parent = 0);
    Activity(QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent = 0);

    // Attribute getters
    QString name();
    int projectId();
    QDateTime startedAt();
    QDateTime endedAt();

    // Attribute setters
    void setName(QString name);
    void setProjectId(int projectId);
    void setStartedAt(QDateTime startedAt);
    void setEndedAt(QDateTime endedAt);
    void setFromParams(const QList<QPair<QString, QString> > &params);

    // Non-attribute getters/setters
    bool isRunning();
    void setRunning(bool running);
    QString nameWithProject();
    void setNameWithProject(QString nameWithProject);

    // Helpers
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
    QVariantMap toVariantMap();

    bool save();

  private:
    static const QString distinctNamesQuery;
    static const QString stopCurrentQuery;
    static const QString deleteShortQuery;

    bool validate();
    QVariant running;
};

#endif
