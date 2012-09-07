#include <QWebSettings>
#include <QUrl>
#include <QKeySequence>
#include <QMessageBox>
#include <QLayout>
#include <QMenuBar>
#include <QtDebug>
#include "window.h"
#include "browser.h"
#include "popup.h"
#include "about.h"

Window::Window(int port, QWidget *parent)
  : QMainWindow(parent), port(port)
{
  layout()->setContentsMargins(0, 0, 0, 0);
  browser = new Browser(this);
  browser->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
  connect(browser, SIGNAL(titleChanged(QString)), this, SLOT(viewTitleChanged(QString)));
  connect(browser, SIGNAL(resized(int, int)), this, SLOT(browserResized(int, int)));

  setCentralWidget(browser);
  setWindowIcon(QIcon(":/icons/hourglass.png"));

  createActions();
  createMenus();
}

void Window::go()
{
  QUrl url("http://127.0.0.1/");
  url.setPort(port);
  browser->load(url);
}

void Window::browserResized(int width, int height)
{
  resize(width, height);
}

void Window::viewTitleChanged(const QString &title)
{
  setWindowTitle(title);
}

void Window::editPreferences()
{
  QUrl url("http://127.0.0.1/settings/edit");
  url.setPort(port);
  Popup *popup = new Popup(url, this);
  popup->exec();
}

void Window::about()
{
  AboutDialog about(this);
  about.exec();
}

void Window::aboutQt()
{
  QMessageBox::aboutQt(this);
}

void Window::createActions()
{
  quitAct = new QAction(tr("&Exit"), this);
  quitAct->setShortcuts(QKeySequence::Quit);
  connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

  preferencesAct = new QAction(tr("&Preferences"), this);
  preferencesAct->setShortcuts(QKeySequence::Preferences);
  connect(preferencesAct, SIGNAL(triggered()), this, SLOT(editPreferences()));

  aboutAct = new QAction(tr("&About"), this);
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  aboutQtAct = new QAction(tr("About &Qt"), this);
  connect(aboutQtAct, SIGNAL(triggered()), this, SLOT(aboutQt()));
}

void Window::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(quitAct);

  editMenu = menuBar()->addMenu(tr("&Edit"));
  editMenu->addAction(preferencesAct);

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);
}
