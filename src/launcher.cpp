#include <QWebSettings>
#include <QUrl>
#include <QtDebug>
#include "launcher.h"
#include "browser.h"

Launcher::Launcher(QWidget *parent)
  : QWidget(parent)
{
  view = new Browser(this);
  view->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
  connect(view, SIGNAL(titleChanged(QString)), this, SLOT(viewTitleChanged(QString)));
}

void Launcher::go()
{
  view->load(QUrl("http://127.0.0.1:5678/"));
  connect(view, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
}

void Launcher::loadFinished(bool ok)
{
  if (ok) {
    qDebug() << "Page finished loading.";
  }
  else {
    qDebug() << "Couldn't load page!";
  }
}

void Launcher::viewTitleChanged(const QString &title)
{
  setWindowTitle(title);
}
