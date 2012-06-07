#include <QtDebug>
#include "server.h"
#include "controller.h"

Server::Server(QString root, quint16 port, QObject *parent)
  : QObject(parent), port(port)
{
  http = new QHttpServer(this);
  connect(http, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
      this, SLOT(handle(QHttpRequest*, QHttpResponse*)));
  this->root = QDir(root);
  qDebug() << "Root directory:" << this->root.absolutePath() << this->root.exists();
}

void Server::handle(QHttpRequest *req, QHttpResponse *resp)
{
  Controller *controller = new Controller(root, req, resp, this);
  connect(controller, SIGNAL(done()), controller, SLOT(deleteLater()));
}

void Server::start()
{
  http->listen(port);
  emit started();
}

void Server::stop()
{
  emit stopped();
}
