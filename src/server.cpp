#include <QByteArray>
#include "server.h"

Server::Server(QString port)
{
  this->ctx = NULL;
  this->port = port;
}

void *Server::router(enum mg_event event, struct mg_connection *conn, const struct mg_request_info *request_info)
{
  if (event == MG_NEW_REQUEST) {
    // Echo requested URI back to the client
    mg_printf(conn, "HTTP/1.1 200 OK\r\n"
              "Content-Type: text/plain\r\n\r\n"
              "%s", request_info->uri);
    return (void *) "";  // Mark as processed
  } else {
    return NULL;
  }
}

bool Server::start()
{
  QByteArray ba = port.toLocal8Bit();
  const char *options[] = {"listening_ports", ba.data(), NULL};

  ctx = mg_start(&router, NULL, options);
  if (ctx == NULL) {
    return false;
  }
  return true;
}

bool Server::stop()
{
  if (ctx != NULL) {
    mg_stop(ctx);
    ctx = NULL;
  }
}
