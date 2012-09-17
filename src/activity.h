#ifndef __ACTIVITY_H
#define __ACTIVITY_H

#include <QSqlDatabase>
#include <QString>
#include <QDateTime>
#include <QDate>
#include <QList>
#include <QPair>
#include <QMap>
#include <QTimer>
#include "record.h"
#include "project.h"
#include "tag.h"

class Activity : public Record
{
  Q_OBJECT

  public:
    static QList<Activity *> find(QObject *parent = 0);
    static QList<Activity *> find(const QString &conditions, QObject *parent = 0);
    static QList<Activity *> find(const QString &conditions, const QString &predicate, QObject *parent = 0);
    static QList<Activity *> findById(int id, QObject *parent = 0);
    static QList<Activity *> findCurrent(QObject *parent = 0);
    static QList<Activity *> findToday(QObject *parent = 0);
    static QList<Activity *> findDay(const QDate &date, QObject *parent = 0);
    static QList<Activity *> findPeriod(const QDate &startDate, const QDate &endDate, QObject *parent = 0);

    static int count();
    static int count(const QString &conditions);
    static int countChangesSince(const QDateTime &dateTime);
    static int countChangesSince(const QDate &day, const QDateTime &dateTime);

    static QMap<QString, int> projectTotals(const QList<Activity *> &activities);
    static QList<QString> distinctNames();
    static void stopCurrent();
    static QVariantList toVariantList(const QList<Activity *> &activities);
    static bool startLike(const Activity *activity);
    static QPair<QDateTime, QDateTime> lastGap();

    Activity(QObject *parent = 0);
    Activity(const QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent = 0);

    // Attribute getters
    QString name() const;
    int projectId() const;
    QDateTime startedAt() const;
    QDateTime endedAt() const;
    bool isUntimed() const;
    int duration() const;
    QDate day() const;

    // Attribute setters
    void setName(const QString &name);
    void setProjectId(int projectId);
    void setStartedAt(const QDateTime &startedAt);
    void setEndedAt(const QDateTime &endedAt);
    void setUntimed(bool untimed);
    void setDuration(int duration);
    void setDay(const QDate &day);
    void setFromParams(const QList<QPair<QString, QString> > &params);

    // Non-attribute getters/setters
    bool isRunning() const;
    void setRunning(bool running);
    bool wasRunning() const;
    QString nameWithProject() const;
    void setNameWithProject(const QString &nameWithProject);
    QString startedAtMDY() const;
    void setStartedAtMDY(const QString &mdy);
    QString startedAtHM() const;
    void setStartedAtHM(const QString &hm);
    QString endedAtMDY() const;
    void setEndedAtMDY(const QString &mdy);
    QString endedAtHM() const;
    void setEndedAtHM(const QString &hm);
    QString dayMDY() const;
    void setDayMDY(const QString &mdy);
    QString tagNames() const;
    void setTagNames(const QString &tagNames);

    // Helpers
    Project *project(QObject *parent = 0) const;
    QString projectName() const;
    QList<Tag *> tags(QObject *parent = 0) const;
    QString startedAtISO8601() const;
    QString endedAtISO8601() const;
    QString durationInWords() const;
    QVariantMap toVariantMap() const;
    bool isSimilarTo(const Activity *other) const;
    bool occursOn(const QDate &date) const;

    bool save();
    bool destroy();

  signals:
    void durationChanged();

  private slots:
    void startDurationTimer();

  private:
    static const QString s_tableName;
    static const QString s_distinctNamesQuery;
    static const QString s_stopCurrentQuery;
    static const QString s_deleteShortQuery;
    static const QString s_addTagQuery;
    static const QString s_removeTagQuery;
    static const QString s_findPeriodConditionsTemplate;
    static const QString s_defaultQueryPredicate;
    static const QString s_findCurrentConditions;
    static const QString s_findDayConditionsTemplate;
    static const QString s_countChangesSinceConditionsTemplate;
    static const QString s_countChangesSinceWithDayConditionsTemplate;

    static QDate dateFromMDY(const QString &mdy);
    static QTime timeFromHM(const QString &hm);

    QVariant m_running;
    bool m_wasRunning;
    QDate m_startedAtMDY;
    QTime m_startedAtHM;
    QDate m_endedAtMDY;
    QTime m_endedAtHM;
    QList<Tag *> m_tagsToAdd;
    QTimer *m_durationTimer;

    void setupDurationTimer();
    void addTags(const QList<Tag *> &tags);
    void removeTags(const QList<Tag *> &tags);

    void beforeValidation();
    bool validate();
    void beforeSave();
    void afterCreate();
    void afterSave();
};

#endif
