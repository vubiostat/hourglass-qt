#ifndef BROWSER_H
#define BROWSER_H

#include <QWebView>
#include <QWebPage>

class Browser : public QWebView
{
  Q_OBJECT

public:
    Browser(QWidget *parent = 0);

private:
    QWebView * createWindow(QWebPage::WebWindowType type);
};

#endif
