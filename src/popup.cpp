#include <QWebView>
#include <QWebSettings>
#include "popup.h"

Popup::Popup(QWidget *parent)
  : QDialog(parent, 0)
{
  view = new QWebView(this);
  view->settings()->setAttribute(QWebSettings::JavascriptCanCloseWindows, true);
  view->show();

  connect(view, SIGNAL(titleChanged(QString)), this, SLOT(viewTitleChanged(QString)));
  connect(view->page(), SIGNAL(windowCloseRequested()), this, SLOT(pageWindowCloseRequested()));
}

void Popup::viewTitleChanged(const QString &title)
{
  setWindowTitle(title);
}

void Popup::pageWindowCloseRequested()
{
  destroy();
}
