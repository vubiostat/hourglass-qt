#include <QString>
#include <QTimer>
#include "thread.h"

ServerThread::ServerThread(int port, QString root, QObject *parent)
  : QThread(parent), port(port), root(root)
{
}

void ServerThread::run()
{
  s = new Server(root, port);
  connect(s, SIGNAL(started()), this, SLOT(serverStartedInternal()));
  connect(s, SIGNAL(stopped()), this, SLOT(serverStoppedInternal()));
  connect(this, SIGNAL(finished()), s, SLOT(stop()));

  s->start();
  exec();
  qDebug() << "Stopping server...";
  s->stop();
}

void ServerThread::serverStartedInternal()
{
  emit serverStarted();
}

void ServerThread::serverStoppedInternal()
{
  qDebug() << "Server stopped.";
  delete s;
}
