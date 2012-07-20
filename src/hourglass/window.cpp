#include <QWebSettings>
#include <QUrl>
#include <QtDebug>
#include "window.h"
#include "browser.h"

Window::Window(int port, QWidget *parent)
  : QMainWindow(parent), port(port)
{
  layout()->setContentsMargins(0, 0, 0, 0);
  browser = new Browser(this);
  browser->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
  connect(browser, SIGNAL(titleChanged(QString)), this, SLOT(viewTitleChanged(QString)));
  connect(browser, SIGNAL(resized(int, int)), this, SLOT(browserResized(int, int)));
  setCentralWidget(browser);

  setWindowIcon(QIcon(":/icons/hourglass.png"));
}

void Window::go()
{
  QUrl url("http://127.0.0.1/");
  url.setPort(port);
  browser->load(url);
}

void Window::browserResized(int width, int height)
{
  resize(width, height);
}

void Window::viewTitleChanged(const QString &title)
{
  setWindowTitle(title);
}
