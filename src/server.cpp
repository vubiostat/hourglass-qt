#include <QString>
#include <QByteArray>
#include <QUrl>
#include "server.h"

const QRegExp Server::editActivityPath = QRegExp("^/activities/(\d+)/edit$");
const QRegExp Server::activityPath = QRegExp("^/activities/(\d+)$");
const QRegExp Server::deleteActivityPath = QRegExp("^/activities/(\d+)/delete$");
const QRegExp Server::restartActivityPath = QRegExp("^/activities/(\d+)/restart$");

Server::Server(QString port)
{
  this->ctx = NULL;
  this->port = port;
}

void *Server::route(enum mg_event event, struct mg_connection *conn, const struct mg_request_info *request_info)
{
  if (event != MG_NEW_REQUEST) {
    return NULL;
  }

  QUrl url(request_info->uri);
  QString path = url.path();
  QString method(request_info->request_method);

  if (method == "GET") {
    if (path == "/") {
    }
    else if (path == "/activities/new") {
    }
    else if (path.contains(editActivityPath)) {
    }
    else if (path.contains(deleteActivityPath)) {
    }
    else if (path == "/activities") {
    }
    else if (path == "/activities/current/stop") {
    }
    else if (path == "/tags") {
    }
    else if (path == "/projects") {
    }
  }
  else if (method == "POST") {
    if (path == "/activities") {
    }
    else if (path.contains(activityPath)) {
    }
    else if (path.contains(restartActivityPath)) {
    }
  }
  return (void *) "";  // Mark as processed
}

bool Server::start()
{
  QByteArray ba = port.toLocal8Bit();
  const char *options[] = {"listening_ports", ba.data(), NULL};

  ctx = mg_start(&route, this, options);
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
