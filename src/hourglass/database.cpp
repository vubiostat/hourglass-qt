#include <QString>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include <QtDebug>
#include <QVariant>
#include <QProcessEnvironment>
#include "database.h"

int DatabaseManager::CURRENT_DATABASE_VERSION = 3;

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

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QDir *path = NULL;
    QString databaseName;
    bool inMemory = false;
    if (env.contains("HOURGLASS_PATH")) {
      path = new QDir(env.value("HOURGLASS_PATH"));
    }
#ifdef Q_OS_LINUX
    else {
      QDir home(QDir::home().path());
      if (!home.exists(".hourglass") && !home.mkdir(".hourglass")) {
        qDebug() << "Can't create directory:" << home.absoluteFilePath(".hourglass");
        inMemory = true;
      }
      else {
        path = new QDir(home.absoluteFilePath(".hourglass"));
      }
    }
#endif

    if (path != NULL && !path->isReadable()) {
      qDebug() << "Directory is not readable:" << path->absolutePath();
      inMemory = true;
      delete path;
    }

    if (inMemory) {
      qDebug() << "Using in-memory database instead.";
      database.setDatabaseName(":memory:");
    }
    else if (path != NULL) {
      database.setDatabaseName(path->absoluteFilePath("hourglass.db"));
      delete path;
    }
    else {
      // Non-Linux
      database.setDatabaseName("hourglass.db");
    }

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
        break;
      case 1:
        database.exec("CREATE TABLE settings (id INTEGER PRIMARY KEY AUTOINCREMENT, key TEXT, value TEXT);");
        database.exec("INSERT INTO settings (key, value) VALUES ('theme', 'smoothness');");
        database.exec("INSERT INTO settings (key, value) VALUES ('day_start', '08:00');");
        database.exec("INSERT INTO settings (key, value) VALUES ('day_end', '18:00');");
        break;
      case 2:
        database.exec("ALTER TABLE activities ADD COLUMN untimed INTEGER DEFAULT 0");
        database.exec("ALTER TABLE activities ADD COLUMN duration INTEGER");
        database.exec("ALTER TABLE activities ADD COLUMN day TEXT");
        break;
    }
    if (database.lastError().isValid()) {
      qDebug() << database.lastError().text();
      break;
    }
    version++;
    if (version == 1) {
      database.exec("INSERT INTO schema_info VALUES (1);");
    }
    else {
      database.exec(QString("UPDATE schema_info SET version = %1;").arg(version));
    }
  }
}
