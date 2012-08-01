#include <QtDebug>
#include <QDir>
#include <QFile>
#include <QFileInfoList>
#include <QByteArray>
#include <sys/types.h>
#include <sys/socket.h>
#include <ctemplate/template.h>
#include "hourglass.h"

int Hourglass::sigintFd[2] = {0, 0};

Hourglass::Hourglass(int &argc, char **argv)
  : QApplication(argc, argv)
{
  // Parse some arguments
  serverOnly = false;
  port = 5678;
  QStringList args = arguments();
  for (int i = 1; i < args.count(); i++) {
    if (args[i] == "--server-only") {
      serverOnly = true;
      continue;
    }
    else if (args[i].startsWith("--port=")) {
      QRegExp rx("^--port=(.+)$");
      bool ok = false;
      if (args[i].contains(rx)) {
        int p = rx.cap(1).toInt(&ok);
        if (ok) {
          port = p;
        }
        else {
          qDebug() << "Invalid port:" << rx.cap(1);
        }
        continue;
      }
    }
    qDebug() << "Invalid argument:" << args[i];
  }

  connect(this, SIGNAL(aboutToQuit()), this, SLOT(cleanUp()));

  // Setup server
  st = new ServerThread(port, ":/public", this);

  // Setup Ctemplate
  QDir views(":/views");
  views.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
  QFileInfoList list = views.entryInfoList();
  for (int i = 0; i < list.size(); ++i) {
    QFileInfo fileInfo = list.at(i);
    QFile file(fileInfo.absoluteFilePath());
    file.open(QIODevice::ReadOnly);
    QByteArray bytes = file.readAll();
    file.close();

    ctemplate::StringToTemplateCache(fileInfo.fileName().toStdString(),
        bytes.data(), bytes.size(), ctemplate::DO_NOT_STRIP);
  }

  // Setup main window
  if (!serverOnly) {
    window = new Window(port);
    connect(st, SIGNAL(serverStarted()), window, SLOT(go()));
  }

  if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigintFd))
    qFatal("Couldn't create INT socketpair");

  snInt = new QSocketNotifier(sigintFd[1], QSocketNotifier::Read, this);
  connect(snInt, SIGNAL(activated(int)), this, SLOT(handleSigInt()));
}

int Hourglass::exec()
{
  connect(st, SIGNAL(finished()), this, SLOT(quit()));
  st->start();
  if (!serverOnly) {
    window->show();
  }

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
