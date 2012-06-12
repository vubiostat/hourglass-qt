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
    static Activity findById(int id);
    static QList<Activity> findCurrent();
    static QList<Activity> findToday();
    static QList<Activity> findDay(QDate date);
    static QMap<QString, int> projectTotals(QList<Activity> &activities);
    static QList<QString> distinctNames();
    static void stopCurrent();
    static QVariantList toVariantList(QList<Activity> &activities);
    static bool startLike(const Activity &activity);

    Activity(QObject *parent = 0);
    Activity(QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent = 0);

    // Attribute getters
    QString name() const;
    int projectId() const;
    QDateTime startedAt() const;
    QDateTime endedAt() const;

    // Attribute setters
    void setName(QString name);
    void setProjectId(int projectId);
    void setStartedAt(QDateTime startedAt);
    void setEndedAt(QDateTime endedAt);
    void setFromParams(const QList<QPair<QString, QString> > &params);

    // Non-attribute getters/setters
    bool isRunning() const;
    void setRunning(bool running);
    QString nameWithProject() const;
    void setNameWithProject(QString nameWithProject);
    QString startedAtMDY() const;
    void setStartedAtMDY(const QString &mdy);
    QString startedAtHM() const;
    void setStartedAtHM(const QString &hm);
    QString endedAtMDY() const;
    void setEndedAtMDY(const QString &mdy);
    QString endedAtHM() const;
    void setEndedAtHM(const QString &hm);

    // Helpers
    Project project() const;
    QString projectName() const;
    QString tagNames() const;
    QString startedAtISO8601() const;
    QString endedAtISO8601() const;
    int duration() const;
    QString durationInWords() const;
    QVariantMap toVariantMap() const;
    bool isSimilarTo(const Activity &other) const;

    bool save();
    bool destroy();
    void beforeValidation();

  private:
    static const QString distinctNamesQuery;
    static const QString stopCurrentQuery;
    static const QString deleteShortQuery;
    static QDate dateFromMDY(const QString &mdy);
    static QTime timeFromHM(const QString &hm);

    QVariant m_running;
    QDate m_startedAtMDY;
    QTime m_startedAtHM;
    QDate m_endedAtMDY;
    QTime m_endedAtHM;
    bool validate();
};

#endif
