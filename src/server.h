#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QString>
#include <QRegExp>
#include <QSqlDatabase>
#include "mongoose.h"

class Server : public QObject
{
  Q_OBJECT

  public:
    Server(QString port);
    bool start();
    bool stop();

    static const QRegExp activityPath;
    static const QRegExp editActivityPath;
    static const QRegExp deleteActivityPath;
    static const QRegExp restartActivityPath;

  private:
    struct mg_context *ctx;
    QString port;

    static void *route(enum mg_event event, struct mg_connection *conn, const struct mg_request_info *request_info);
    QString index();
};

#endif
