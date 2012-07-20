#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include "browser.h"

class Window : public QMainWindow
{
  Q_OBJECT

  public:
    Window(int port, QWidget *parent = 0);

  public slots:
    void go();

  private slots:
    void browserResized(int width, int height);
    void viewTitleChanged(const QString &title);

  private:
    int port;
    Browser *browser;
};

#endif
