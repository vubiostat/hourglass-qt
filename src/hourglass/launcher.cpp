#include <QWebSettings>
#include <QUrl>
#include <QtDebug>
#include "launcher.h"
#include "browser.h"

Launcher::Launcher(QWidget *parent)
  : QWidget(parent)
{
  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  browser = new Browser(this);
  browser->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
  connect(browser, SIGNAL(titleChanged(QString)), this, SLOT(viewTitleChanged(QString)));
  connect(browser, SIGNAL(resized(int, int)), this, SLOT(browserResized(int, int)));
  layout->addWidget(browser);
  firstLoad = true;
}

void Launcher::go()
{
  browser->load(QUrl("http://127.0.0.1:5678/"));
}

void Launcher::browserResized(int width, int height)
{
  resize(width, height);
}

void Launcher::viewTitleChanged(const QString &title)
{
  setWindowTitle(title);
}
