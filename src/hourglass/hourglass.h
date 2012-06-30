#ifndef HOURGLASS_H
#define HOURGLASS_H

#include <QApplication>
#include <QSocketNotifier>
#include "launcher.h"
#include "thread.h"

class Hourglass : public QApplication
{
  Q_OBJECT

  public:
    Hourglass(int &argc, char **argv);
    int exec();

    static void intSignalHandler(int unused);

  public slots:
    void handleSigInt();
    void cleanUp();

  private:
    bool serverOnly;
    int port;
    Launcher *launcher;
    ServerThread *st;

    static int sigintFd[2];
    QSocketNotifier *snInt;
};

#endif
