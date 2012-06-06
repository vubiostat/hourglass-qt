#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include "server.h"

class ServerThread : public QThread
{
  Q_OBJECT

  public:
    ServerThread(QString root, QObject *parent = 0);
    void run();

  signals:
    void serverStarted();

  private:
    QString root;
    Server *s;

  private slots:
    void serverStartedInternal();
    void serverStoppedInternal();
};

#endif
