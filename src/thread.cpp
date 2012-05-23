#include <QString>
#include "thread.h"

void ServerThread::run()
{
  s = new Server(QString("4567"));
  if (s->start()) {
    exec();
    s->stop();
    delete s;
  }
}
