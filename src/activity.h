#ifndef __ACTIVITY_H
#define __ACTIVITY_H

#include <QSqlDatabase>
#include <QString>
#include <QDateTime>
#include <QDate>
#include <QList>
#include <QPair>
#include <QMap>
#include "record.h"
#include "project.h"
#include "tag.h"

class Activity : public Record
{
  Q_OBJECT

  public:
    static QList<Activity> find();
    static QList<Activity> find(QString conditions);
    static QList<Activity> find(QString conditions, QString predicate);
    static Activity findById(int id);
    static QList<Activity> findCurrent();
    static QList<Activity> findToday();
    static QList<Activity> findDay(QDate date);
    static QList<Activity> findPeriod(const QDate &startDate, const QDate &endDate);
    static QMap<QString, int> projectTotals(QList<Activity> &activities);
    static QList<QString> distinctNames();
    static void stopCurrent();
    static QVariantList toVariantList(QList<Activity> &activities);
    static bool startLike(const Activity &activity);
    static QPair<QDateTime, QDateTime> lastGap();

    Activity(QObject *parent = 0);
    Activity(QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent = 0);

    // Attribute getters
    QString name() const;
    int projectId() const;
    QDateTime startedAt() const;
    QDateTime endedAt() const;
    bool isUntimed() const;
    int duration() const;
    QDate day() const;

    // Attribute setters
    void setName(QString name);
    void setProjectId(int projectId);
    void setStartedAt(QDateTime startedAt);
    void setEndedAt(QDateTime endedAt);
    void setUntimed(bool untimed);
    void setDuration(int duration);
    void setDay(QDate day);
    void setFromParams(const QList<QPair<QString, QString> > &params);

    // Non-attribute getters/setters
    bool isRunning() const;
    void setRunning(bool running);
    bool wasRunning() const;
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
    QString dayMDY() const;
    void setDayMDY(const QString &mdy);
    QString tagNames() const;
    void setTagNames(const QString &tagNames);

    // Helpers
    Project project() const;
    QString projectName() const;
    QList<Tag> tags() const;
    QString startedAtISO8601() const;
    QString endedAtISO8601() const;
    QString durationInWords() const;
    QVariantMap toVariantMap() const;
    bool isSimilarTo(const Activity &other) const;
    bool occursOn(const QDate &date) const;

    bool save();
    bool destroy();

  private:
    static const QString distinctNamesQuery;
    static const QString stopCurrentQuery;
    static const QString deleteShortQuery;
    static const QString addTagQuery;
    static const QString removeTagQuery;
    static const QString s_findPeriodQueryTemplate;
    static QDate dateFromMDY(const QString &mdy);
    static QTime timeFromHM(const QString &hm);

    QVariant m_running;
    QVariant m_wasRunning;
    QDate m_startedAtMDY;
    QTime m_startedAtHM;
    QDate m_endedAtMDY;
    QTime m_endedAtHM;
    QList<Tag> m_tagsToAdd;

    void addTags(const QList<Tag> &tags);
    void removeTags(const QList<Tag> &tags);

    void beforeValidation();
    bool validate();
    void beforeSave();
    void afterCreate();
};

#endif
