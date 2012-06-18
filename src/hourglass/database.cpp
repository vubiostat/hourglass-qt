#include <QString>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>
#include <QVariant>
#include "database.h"

int DatabaseManager::CURRENT_DATABASE_VERSION = 1;

DatabaseManager::DatabaseManager(QObject *parent)
  : QObject(parent)
{
}

DatabaseManager::~DatabaseManager()
{
  if (database.isOpen()) {
    database.close();
  }
}

QSqlDatabase &DatabaseManager::getDatabase()
{
  if (!database.isValid()) {
    database = QSqlDatabase::addDatabase("QSQLITE");

#ifdef Q_OS_LINUX
    QDir path(QDir::home().path());
    bool inMemory = false;
    if (!path.exists(".hourglass") && !path.mkdir(".hourglass")) {
      qDebug() << "Can't create directory:" << path.absoluteFilePath(".hourglass");
      inMemory = true;
    }
    else {
      if (!path.cd(".hourglass") || !path.isReadable()) {
        qDebug() << "Directory is not readable:" << path.absolutePath();
        inMemory = true;
      }
      else {
        database.setDatabaseName(path.absoluteFilePath("hourglass.db"));
      }
    }

    if (inMemory) {
      qDebug() << "Using in-memory database instead.";
      database.setDatabaseName(":memory:");
    }
#else
    database.setDatabaseName("hourglass.db");
#endif

    database.open();
    migrateDatabase();
  }
  return database;
}

void DatabaseManager::migrateDatabase()
{
  int version = 0;

  QSqlQuery query = database.exec("SELECT version FROM schema_info");
  while (query.next()) {
    version = query.value(0).toInt();
  }
  //qDebug() << "Version:" << version;

  while (version < CURRENT_DATABASE_VERSION) {
    switch (version) {
      case 0:
        database.exec("CREATE TABLE schema_info (version INTEGER);");
        database.exec("CREATE TABLE projects (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT);");
        database.exec("CREATE TABLE activities (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, project_id INTEGER REFERENCES projects(id) ON DELETE SET NULL, started_at TEXT, ended_at TEXT);");
        database.exec("CREATE TABLE tags (id INTEGER PRIMARY KEY, name TEXT);");
        database.exec("CREATE TABLE activities_tags (activity_id INTEGER REFERENCES activities(id) ON DELETE CASCADE, tag_id INTEGER REFERENCES tags(id) ON DELETE CASCADE);");
        database.exec("INSERT INTO schema_info VALUES (1);");
        break;
    }
    if (database.lastError().isValid()) {
      qDebug() << database.lastError().text();
      break;
    }
    version++;
  }
}
