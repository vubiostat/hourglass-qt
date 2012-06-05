#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include "server.h"

class ServerThread : public QThread
{
  Q_OBJECT

  public:
    void run();

  signals:
    void serverStarted();

  private:
    Server *s;

  private slots:
    void serverStartedInternal();
    void serverStoppedInternal();
};

#endif
