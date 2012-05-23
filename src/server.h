#ifndef SERVER_H
#define SERVER_H

#include <QString>
#include <QRegExp>
#include "mongoose.h"

class Server
{
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
};

#endif
