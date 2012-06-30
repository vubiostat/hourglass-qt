#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QWidget>
#include <QVBoxLayout>
#include "browser.h"

class Launcher : public QWidget
{
  Q_OBJECT

  public:
    Launcher(int port, QWidget *parent = 0);

  public slots:
    void go();

  private slots:
    void browserResized(int width, int height);
    void viewTitleChanged(const QString &title);

  private:
    int port;
    QVBoxLayout *layout;
    Browser *browser;
    bool firstLoad;
};

#endif
