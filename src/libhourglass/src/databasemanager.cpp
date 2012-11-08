#include <QString>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include <QSettings>
#include "hourglass/databasemanager.h"

int DatabaseManager::CURRENT_DATABASE_VERSION = 4;
QSqlDatabase DatabaseManager::s_database;

QSqlDatabase &DatabaseManager::getDatabase()
{
  bool ok = true;
  QSettings settings;

  if (!s_database.isValid()) {
    s_database = QSqlDatabase::addDatabase("QSQLITE");

    if (settings.contains("databasePath")) {
      s_database.setDatabaseName(settings.value("databasePath").toString());
    }
    else {
#ifdef Q_OS_LINUX
      QDir path(QDir::home().path());
      if ((path.exists(".hourglass") || path.mkdir(".hourglass")) && path.cd(".hourglass")) {
        QString databasePath = path.absoluteFilePath("hourglass.db");
        s_database.setDatabaseName(databasePath);
        settings.setValue("databasePath", databasePath);
      }
#else
      // Non-Linux
      s_database.setDatabaseName("hourglass.db");
#endif
    }

    if (s_database.databaseName().isEmpty() || !s_database.open()) {
      // Try to use in-memory database
      s_database.setDatabaseName(":memory:");
      if (s_database.open()) {
        qWarning("Couldn't create a local database; using an in-memory s_database instead. Your activities will not be saved.");
      }
      else {
        qCritical("Couldn't create a local database or use an in-memory s_database.");
        ok = false;
      }
    }

    if (ok) {
      ok = migrateDatabase(s_database);
    }

    if (!ok) {
      s_database = QSqlDatabase();
    }
  }
  return s_database;
}

bool DatabaseManager::migrateDatabase(QSqlDatabase &database)
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
      case 3:
        database.exec("DROP TABLE settings");
        database.exec("ALTER TABLE projects ADD COLUMN created_at TEXT");
        database.exec("ALTER TABLE projects ADD COLUMN updated_at TEXT");
        database.exec("ALTER TABLE activities ADD COLUMN created_at TEXT");
        database.exec("ALTER TABLE activities ADD COLUMN updated_at TEXT");
        database.exec("ALTER TABLE tags ADD COLUMN created_at TEXT");
        database.exec("ALTER TABLE tags ADD COLUMN updated_at TEXT");
        break;
    }
    if (database.lastError().isValid()) {
      QByteArray tmp = database.lastError().text().toLatin1();
      qCritical("%s", tmp.constData());
      return false;
    }
    version++;
    if (version == 1) {
      database.exec("INSERT INTO schema_info VALUES (1);");
    }
    else {
      database.exec(QString("UPDATE schema_info SET version = %1;").arg(version));
    }
  }
  return true;
}
