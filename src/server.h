#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QString>
#include <QRegExp>
#include <QSqlDatabase>
#include <QList>
#include <QPair>
#include "mongoose.h"
#include "activity.h"
#include "dictionary.h"

class Server : public QObject
{
  Q_OBJECT

  public:
    Server(QString root, QString port, QObject *parent = 0);
    void start();

    static const QRegExp activityPath;
    static const QRegExp editActivityPath;
    static const QRegExp deleteActivityPath;
    static const QRegExp restartActivityPath;
    static QList<QPair<QString, QString> > decodePost(const char *data);

  signals:
    void started();
    void stopped();

  public slots:
    void stop();

  private:
    struct mg_context *ctx;
    QString root, port;

    static void *route(enum mg_event event, struct mg_connection *conn, const struct mg_request_info *request_info);

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
};

#endif
