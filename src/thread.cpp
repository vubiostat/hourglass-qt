#include <QString>
#include <QTimer>
#include "thread.h"

void ServerThread::run()
{
  s = new Server("5678");
  connect(s, SIGNAL(started()), this, SLOT(serverStartedInternal()));
  connect(s, SIGNAL(stopped()), this, SLOT(serverStoppedInternal()));
  connect(this, SIGNAL(finished()), s, SLOT(stop()));

  s->start();
  exec();
  s->stop();
}

void ServerThread::serverStartedInternal()
{
  emit serverStarted();
}

void ServerThread::serverStoppedInternal()
{
  delete s;
}
