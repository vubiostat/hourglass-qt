#ifndef POPUP_H
#define POPUP_H

#include <QVBoxLayout>
#include <QDialog>
#include <QUrl>
#include "browser.h"

class Popup : public QDialog
{
  Q_OBJECT

  public:
    Popup(QWidget *parent = 0);
    Popup(const QUrl &location, QWidget *parent = 0);
    Browser *browser;

  private slots:
    void viewTitleChanged(const QString &title);
    void pageWindowCloseRequested();
    void browserResized(int width, int height);

  private:
    QVBoxLayout *layout;

    void setup();
};

#endif
