#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QDir>
#include <QObject>
#include <QByteArray>
#include <qhttpserver.h>
#include <qhttprequest.h>
#include <qhttpresponse.h>
#include "dictionary.h"
#include "activity.h"

class Controller : public QObject
{
  Q_OBJECT

  public:
    Controller(QDir root, QHttpRequest *req, QHttpResponse *resp, QObject *parent = 0);
    static QList<QPair<QString, QString> > decodePost(const char *data);

  signals:
    void done();

  private slots:
    void accumulate(const QByteArray &data);
    void route();

  private:
    static const QRegExp activityPath;
    static const QRegExp editActivityPath;
    static const QRegExp deleteActivityPath;
    static const QRegExp restartActivityPath;

    QDir m_root;
    QHttpRequest *m_req;
    QHttpResponse *m_resp;
    QByteArray data;

    void includeActivities(Dictionary *dictionary, QList<Activity> &activities);
    void includeNames(Dictionary *dictionary, QList<QString> &names);

    void includeCurrent(Dictionary *dictionary);
    void includeToday(Dictionary *dictionary);
    void includeWeek(Dictionary *dictionary);
    void includeTotals(Dictionary *dictionary, bool addIncludeDictionary = true);

    QString partialTotals();
    QString partialUpdates();
    QString partialNames(QList<QString> &names);
    QString partialActivityNames();
    QString partialProjectNames();
    QString partialTagNames();

    QString index();
    QString createActivity(const QList<QPair<QString, QString> > &params);
    QString stopCurrentActivities();
    QString newActivity();

    void serveFile(const QString &path);
};

#endif
