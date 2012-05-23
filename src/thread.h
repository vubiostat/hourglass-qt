#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include "server.h"

class ServerThread : public QThread
{
  Q_OBJECT

  public:
    void run();

  private:
    Server *s;
};

#endif
