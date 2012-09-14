#ifndef __HOURGLASS_H
#define __HOURGLASS_H

#include <QApplication>
#include <QSqlDatabase>
#include "mainwindow.h"

#define CURRENT_DATABASE_VERSION 4

class Hourglass : public QApplication
{
  Q_OBJECT

  public:
    Hourglass(int &argc, char **argv);

  private:
    MainWindow *m_mainwindow;

    bool setupDatabase();
    void migrateDatabase(QSqlDatabase &database);
};

#endif
