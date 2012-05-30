#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>

class DatabaseManager : public QObject
{
  Q_OBJECT

  public:
    static int CURRENT_DATABASE_VERSION;
    static DatabaseManager& getInstance() {
      static DatabaseManager instance(0);
      return instance;
    }
    ~DatabaseManager();

  public:
    QSqlDatabase &getDatabase();

  private:
    DatabaseManager(QObject *parent);
    DatabaseManager(DatabaseManager const&); // Not implemented
    void operator=(DatabaseManager const&);  // Not implemented
    void migrateDatabase();

    QSqlDatabase database;
};
#endif
