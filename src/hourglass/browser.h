#ifndef BROWSER_H
#define BROWSER_H

#include <QWebView>
#include <QWebPage>

class Browser : public QWebView
{
  Q_OBJECT

  public:
    Browser(QWidget *parent = 0);

  public slots:
    void resizeView();
    void resizeViewHeight();

  signals:
    void resized(int width, int height);

  private:
    QWebView *createWindow(QWebPage::WebWindowType type);
};

#endif
