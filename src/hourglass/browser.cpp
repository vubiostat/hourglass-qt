#include "browser.h"
#include "popup.h"

Browser::Browser(QWidget *parent)
  : QWebView(parent)
{
}

QWebView * Browser::createWindow(QWebPage::WebWindowType type)
{
  Popup *popup = new Popup(parentWidget());
  return popup->view;
}
