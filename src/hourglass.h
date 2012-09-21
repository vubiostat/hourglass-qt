#ifndef __HOURGLASS_H
#define __HOURGLASS_H

#include <QApplication>
#include <QSettings>
#include <QSqlDatabase>
#include "mainwindow.h"

#define CURRENT_DATABASE_VERSION 4

class Hourglass : public QApplication
{
  Q_OBJECT

  public:
    Hourglass(int &argc, char **argv);

  private slots:
    void stopActivities();

  private:
    MainWindow *m_mainwindow;

    bool setupDatabase();
    void migrateDatabase(QSqlDatabase &database);
};

#endif
