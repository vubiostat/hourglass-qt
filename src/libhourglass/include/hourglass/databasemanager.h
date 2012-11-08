#ifndef __DATABASE_MANAGER_H
#define __DATABASE_MANAGER_H

#include <QObject>
#include <QSqlDatabase>

class DatabaseManager : public QObject
{
  Q_OBJECT

  public:
    static int CURRENT_DATABASE_VERSION;
    static QSqlDatabase &getDatabase();
    static bool migrateDatabase(QSqlDatabase &database);

  private:
    static QSqlDatabase s_database;
};
#endif
