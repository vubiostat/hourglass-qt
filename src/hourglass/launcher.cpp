#include <QWebSettings>
#include <QUrl>
#include <QtDebug>
#include "launcher.h"
#include "browser.h"

Launcher::Launcher(int port, QWidget *parent)
  : QWidget(parent), port(port)
{
  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  browser = new Browser(this);
  browser->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
  connect(browser, SIGNAL(titleChanged(QString)), this, SLOT(viewTitleChanged(QString)));
  connect(browser, SIGNAL(resized(int, int)), this, SLOT(browserResized(int, int)));
  layout->addWidget(browser);
  firstLoad = true;

  setWindowIcon(QIcon(":/icons/hourglass.png"));
}

void Launcher::go()
{
  QUrl url("http://127.0.0.1/");
  url.setPort(port);
  browser->load(url);
}

void Launcher::browserResized(int width, int height)
{
  resize(width, height);
}

void Launcher::viewTitleChanged(const QString &title)
{
  setWindowTitle(title);
}
