#ifndef HOURGLASS_H
#define HOURGLASS_H

#include <QApplication>
#include <QSocketNotifier>
#include "window.h"
#include "thread.h"

class Hourglass : public QApplication
{
  Q_OBJECT

  public:
    Hourglass(int &argc, char **argv);
    int exec();

#ifndef Q_OS_WIN32
    static void intSignalHandler(int unused);
#endif

  public slots:
#ifndef Q_OS_WIN32
    void handleSigInt();
#endif
    void cleanUp();

  private:
    bool serverOnly;
    int port;
    Window *window;
    ServerThread *st;

#ifndef Q_OS_WIN32
    static int sigintFd[2];
    QSocketNotifier *snInt;
#endif
};

#endif
