#include <QWebView>
#include <QWebSettings>
#include "popup.h"

Popup::Popup(QWidget *parent)
  : QDialog(parent, 0)
{
  setup();
}

Popup::Popup(const QUrl &url, QWidget *parent)
  : QDialog(parent, 0)
{
  setup();
  browser->load(url);
}

void Popup::viewTitleChanged(const QString &title)
{
  setWindowTitle(title);
}

void Popup::browserResized(int width, int height)
{
  resize(width, height);
}

void Popup::pageWindowCloseRequested()
{
  destroy();
}

void Popup::setup()
{
  layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  browser = new Browser(this);
  browser->settings()->setAttribute(QWebSettings::JavascriptCanCloseWindows, true);
  connect(browser, SIGNAL(titleChanged(QString)), this, SLOT(viewTitleChanged(QString)));
  connect(browser, SIGNAL(resized(int, int)), this, SLOT(browserResized(int, int)));
  connect(browser->page(), SIGNAL(windowCloseRequested()), this, SLOT(pageWindowCloseRequested()));
  layout->addWidget(browser);
}
