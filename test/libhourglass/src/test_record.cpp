#include "test_record.h"

void TestRecord::initTestCase()
{
  m_database = QSqlDatabase::addDatabase("QSQLITE");
  Record::setDatabase(m_database);
}

void TestRecord::init()
{
  m_database.setDatabaseName(":memory:");
  if (!m_database.open()) {
    QFAIL("Couldn't open database");
  }
}

void TestRecord::cleanup()
{
  m_database.close();
}

void TestRecord::cleanupTestCase()
{
  m_database = QSqlDatabase();
  QSqlDatabase::removeDatabase("QSQLITE");
}

void TestRecord::findIds()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  QList<int> ids = Record::findIds("foo");
  QCOMPARE(ids.size(), 1);
  QCOMPARE(ids[0], 1);
}

void TestRecord::findIdsWithConditions()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar')");

  QList<int> ids = Record::findIds("foo", "WHERE bar = 'bar'");
  QCOMPARE(ids.size(), 1);
  QCOMPARE(ids[0], 2);
}

void TestRecord::findIdsWithConditionsAndPredicate()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar 2'), ('bar 1')");

  QList<int> ids = Record::findIds("foo", "WHERE bar LIKE 'bar%'", "ORDER BY bar");
  QCOMPARE(ids.size(), 2);
  QCOMPARE(ids[0], 3);
  QCOMPARE(ids[1], 2);
}

void TestRecord::findIdsWithConditionsAndBindValues()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar')");

  QList<QVariant> bindValues;
  bindValues << "bar";
  QList<int> ids = Record::findIds("foo", "WHERE bar = ?", bindValues);
  QCOMPARE(ids.size(), 1);
  QCOMPARE(ids[0], 2);
}

void TestRecord::findIdsWithEverything()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar 2'), ('bar 1')");

  QList<QVariant> bindValues;
  bindValues << "bar%";
  QList<int> ids = Record::findIds("foo", "WHERE bar LIKE ?", bindValues, "ORDER BY bar");
  QCOMPARE(ids.size(), 2);
  QCOMPARE(ids[0], 3);
  QCOMPARE(ids[1], 2);
}

void TestRecord::executeQuery(const QString &query)
{
  m_database.exec(query);
  if (m_database.lastError().isValid()) {
    QByteArray tmp = m_database.lastError().text().toLatin1();
    qCritical("%s", tmp.constData());
    QFAIL("Query failed!");
  }
}
