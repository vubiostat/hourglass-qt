#include <QtDebug>
#include <sys/types.h>
#include <sys/socket.h>
#include <ctemplate/template.h>
#include "hourglass.h"

int Hourglass::sigintFd[2] = {0, 0};

Hourglass::Hourglass(int &argc, char **argv)
  : QApplication(argc, argv)
{
  connect(this, SIGNAL(aboutToQuit()), this, SLOT(cleanUp()));

  // Setup Ctemplate
  ctemplate::Template::SetTemplateRootDirectory(VIEW_PATH);

  // Setup server
  st = new ServerThread(PUBLIC_PATH, this);

  if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigintFd))
    qFatal("Couldn't create INT socketpair");

  snInt = new QSocketNotifier(sigintFd[1], QSocketNotifier::Read, this);
  connect(snInt, SIGNAL(activated(int)), this, SLOT(handleSigInt()));
}

int Hourglass::exec()
{
  connect(st, SIGNAL(serverStarted()), &launcher, SLOT(go()));
  connect(st, SIGNAL(finished()), this, SLOT(quit()));
  st->start();

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
  st->quit();
  st->wait();
}
