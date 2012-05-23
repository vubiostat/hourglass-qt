#ifndef SERVER_H
#define SERVER_H

#include <QString>
#include "mongoose.h"

class Server
{
  public:
    Server(QString port);
    bool start();
    bool stop();

  private:
    struct mg_context *ctx;
    QString port;

    static void *router(enum mg_event event, struct mg_connection *conn, const struct mg_request_info *request_info);
};

#endif
