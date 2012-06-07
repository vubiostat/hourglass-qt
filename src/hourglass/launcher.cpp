#include <QWebSettings>
#include <QUrl>
#include <QWebFrame>
#include <QtDebug>
#include "launcher.h"
#include "browser.h"

Launcher::Launcher(QWidget *parent)
  : QWidget(parent)
{
  view = new Browser(this);
  view->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
  connect(view, SIGNAL(titleChanged(QString)), this, SLOT(viewTitleChanged(QString)));
  connect(view, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
  loadCount = 0;
}

void Launcher::go()
{
  view->load(QUrl("http://127.0.0.1:5678/"));
}

void Launcher::loadFinished(bool ok)
{
  if (ok) {
    switch (loadCount++) {
      case 0:
        /* Hackity hack */
        view->reload();
        break;
      case 1:
        view->show();
        show();
        break;
      default:
        break;
    }
  }
  else {
    qDebug() << "Couldn't load page!";
  }
}

void Launcher::viewTitleChanged(const QString &title)
{
  setWindowTitle(title);
}
