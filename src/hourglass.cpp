#include <QtDebug>
#include <sys/types.h>
#include <sys/socket.h>
#include "hourglass.h"

int Hourglass::sigintFd[2] = {0, 0};

Hourglass::Hourglass(int &argc, char **argv)
  : QApplication(argc, argv)
{
  connect(this, SIGNAL(aboutToQuit()), this, SLOT(cleanUp()));

  if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigintFd))
    qFatal("Couldn't create INT socketpair");

  snInt = new QSocketNotifier(sigintFd[1], QSocketNotifier::Read, this);
  connect(snInt, SIGNAL(activated(int)), this, SLOT(handleSigInt()));
}

int Hourglass::exec()
{
  st.start();
  launcher.show();
  connect(&st, SIGNAL(finished()), this, SLOT(quit()));
  return QApplication::exec();
}

void Hourglass::handleSigInt()
{
  snInt->setEnabled(false);
  char tmp;
  ::read(sigintFd[1], &tmp, sizeof(tmp));

  quit();

  snInt->setEnabled(true);
}

void Hourglass::intSignalHandler(int)
{
  char a = 1;
  ::write(sigintFd[0], &a, sizeof(a));
}

void Hourglass::cleanUp()
{
  qDebug() << "Cleaning up...";
  st.quit();
  st.wait();
}
