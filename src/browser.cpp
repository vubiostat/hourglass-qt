#include <QSizePolicy>
#include <QWebFrame>
#include <QVariant>
#include <QtDebug>
#include "browser.h"
#include "popup.h"

Browser::Browser(QWidget *parent)
  : QWebView(parent)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  connect(this, SIGNAL(loadFinished(bool)), this, SLOT(resizeView()));

  QWebFrame *frame = page()->mainFrame();
  frame->addToJavaScriptWindowObject("qt", this);
}

void Browser::resizeView()
{
  QWebFrame *frame = page()->mainFrame();
  int width = frame->evaluateJavaScript("$('body').outerWidth(true);").toInt();
  int height = frame->evaluateJavaScript("$('body').outerHeight(true);").toInt();
  qDebug() << "Width:" << width << "Height:" << height;
  resize(width, height);
  emit resized(width, height);
}

void Browser::resizeViewHeight()
{
  QWebFrame *frame = page()->mainFrame();
  int height = frame->evaluateJavaScript("$('body').outerHeight(true);").toInt();
  qDebug() << "Height:" << height;
  resize(width(), height);
  emit resized(width(), height);
}

QWebView *Browser::createWindow(QWebPage::WebWindowType type)
{
  Popup *popup = new Popup(parentWidget());
  return popup->browser;
}
