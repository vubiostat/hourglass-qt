#ifndef POPUP_H
#define POPUP_H

#include <QDialog>
class QWebView;

class Popup : public QDialog
{
  Q_OBJECT

public:
    Popup(QWidget *parent = 0);
    QWebView *view;

private slots:
    void viewTitleChanged(const QString &title);
    void pageWindowCloseRequested();
};

#endif
