#ifndef __THREAD_H
#define __THREAD_H

#include <QThread>
#include "server.h"

class ServerThread : public QThread
{
  Q_OBJECT

  public:
    ServerThread(int port, QString root, QObject *parent = 0);
    void run();

  signals:
    void serverStarted();

  private:
    int port;
    QString root;
    Server *s;

  private slots:
    void serverStartedInternal();
    void serverStoppedInternal();
};

#endif
