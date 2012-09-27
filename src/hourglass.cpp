#include "hourglass.h"
#include <QSettings>
#include <QMessageBox>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include "activity.h"
#include <QtDebug>

Hourglass::Hourglass(int &argc, char **argv)
  : QApplication(argc, argv)
{
  connect(this, SIGNAL(aboutToQuit()), SLOT(stopActivities()));

  setOrganizationName("vubiostat");
  setOrganizationDomain("biostat.mc.vanderbilt.edu");
  setApplicationName("hourglass");

  if (setupDatabase()) {
    m_mainwindow = new MainWindow();
    setQuitOnLastWindowClosed(!m_mainwindow->isTrayIconShown());
    connect(m_mainwindow, SIGNAL(trayIconHidden()), SLOT(trayIconHidden()));
    connect(m_mainwindow, SIGNAL(trayIconShown()), SLOT(trayIconShown()));
    m_mainwindow->show();
  }
  else {
    quit();
  }
}

void Hourglass::stopActivities()
{
  QSettings settings;
  if (settings.value("stopActivitiesOnExit", true).toBool()) {
    Activity::stopCurrent();
  }
}

void Hourglass::trayIconHidden()
{
  setQuitOnLastWindowClosed(true);
}

void Hourglass::trayIconShown()
{
  setQuitOnLastWindowClosed(false);
}

bool Hourglass::setupDatabase()
{
  bool ok = true;
  QSettings settings;
  QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");

  if (settings.contains("databasePath")) {
    database.setDatabaseName(settings.value("databasePath").toString());
  }
  else {
#ifdef Q_OS_LINUX
    QDir path(QDir::home().path());
    if ((path.exists(".hourglass") || path.mkdir(".hourglass")) && path.cd(".hourglass")) {
      QString databasePath = path.absoluteFilePath("hourglass.db");
      database.setDatabaseName(databasePath);
      settings.setValue("databasePath", databasePath);
    }
#else
    // Non-Linux
    database.setDatabaseName("hourglass.db");
#endif
  }

  if (database.databaseName().isEmpty() || !database.open()) {
    // Try to use in-memory database
    database.setDatabaseName(":memory:");
    if (database.open()) {
      QMessageBox::warning(NULL, "In memory database", "Couldn't create a local database; using an in-memory database instead. Your activities will not be saved.");
    }
    else {
      QMessageBox::critical(NULL, "Database", "Couldn't create a local database or use an in-memory database. Closing...");
      ok = false;
    }
  }

  if (ok) {
    migrateDatabase(database);
  }
  return ok;
}

void Hourglass::migrateDatabase(QSqlDatabase &database)
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
