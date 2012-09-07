#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QAction>
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
    void editPreferences();
    void about();
    void aboutQt();

  private:
    int port;
    Browser *browser;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    QAction *quitAct;
    QAction *preferencesAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    void createActions();
    void createMenus();
};

#endif
