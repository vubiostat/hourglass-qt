#include <QString>
#include <QTimer>
#include "thread.h"

void ServerThread::run()
{
  s = new Server(QString("5678"));
  //QTimer::singleShot(10000, this, SLOT(quit()));
  if (s->start()) {
    exec();
    s->stop();
    delete s;
  }
}
