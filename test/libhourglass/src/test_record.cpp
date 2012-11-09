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
  while (!m_records.isEmpty()) {
    delete m_records.takeLast();
  }
}

void TestRecord::cleanupTestCase()
{
  m_database = QSqlDatabase();
  QSqlDatabase::removeDatabase("QSQLITE");
}

void TestRecord::staticFind()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  m_records = Record::find<Record>("foo");
  QCOMPARE(m_records.size(), 1);
  QCOMPARE(m_records[0]->id(), 1);
  QCOMPARE(m_records[0]->get("bar").toString(), QString("foo"));
}

void TestRecord::staticFindWithParent()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  m_records = Record::find<Record>("foo", this);
  QCOMPARE(m_records.size(), 1);
  QCOMPARE(m_records[0]->parent(), this);
}

void TestRecord::staticFindWithConditions()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar')");

  m_records = Record::find<Record>("foo", "WHERE bar = 'foo'");
  QCOMPARE(m_records.size(), 1);
  QCOMPARE(m_records[0]->id(), 1);
  QCOMPARE(m_records[0]->get("bar").toString(), QString("foo"));
}

void TestRecord::staticFindWithConditionsAndPredicate()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar 2'), ('bar 1')");

  m_records = Record::find<Record>("foo", "WHERE bar LIKE 'bar%'", "ORDER BY bar");
  QCOMPARE(m_records.size(), 2);
  QCOMPARE(m_records[0]->id(), 3);
  QCOMPARE(m_records[0]->get("bar").toString(), QString("bar 1"));
  QCOMPARE(m_records[1]->id(), 2);
  QCOMPARE(m_records[1]->get("bar").toString(), QString("bar 2"));
}

void TestRecord::staticFindWithConditionsAndBindValues()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar')");

  QList<QVariant> bindValues;
  bindValues << "bar";
  m_records = Record::find<Record>("foo", "WHERE bar = ?", bindValues);
  QCOMPARE(m_records.size(), 1);
  QCOMPARE(m_records[0]->id(), 2);
  QCOMPARE(m_records[0]->get("bar").toString(), QString("bar"));
}

void TestRecord::staticFindWithEverything()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar 2'), ('bar 1')");

  QList<QVariant> bindValues;
  bindValues << "bar%";
  m_records = Record::find<Record>("foo", "WHERE bar LIKE ?", bindValues, "ORDER BY bar");
  QCOMPARE(m_records.size(), 2);
  QCOMPARE(m_records[0]->id(), 3);
  QCOMPARE(m_records[0]->get("bar").toString(), QString("bar 1"));
  QCOMPARE(m_records[1]->id(), 2);
  QCOMPARE(m_records[1]->get("bar").toString(), QString("bar 2"));
}

void TestRecord::staticFindIds()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  QList<int> ids = Record::findIds("foo");
  QCOMPARE(ids.size(), 1);
  QCOMPARE(ids[0], 1);
}

void TestRecord::staticFindIdsWithConditions()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar')");

  QList<int> ids = Record::findIds("foo", "WHERE bar = 'bar'");
  QCOMPARE(ids.size(), 1);
  QCOMPARE(ids[0], 2);
}

void TestRecord::staticFindIdsWithConditionsAndPredicate()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar 2'), ('bar 1')");

  QList<int> ids = Record::findIds("foo", "WHERE bar LIKE 'bar%'", "ORDER BY bar");
  QCOMPARE(ids.size(), 2);
  QCOMPARE(ids[0], 3);
  QCOMPARE(ids[1], 2);
}

void TestRecord::staticFindIdsWithConditionsAndBindValues()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar')");

  QList<QVariant> bindValues;
  bindValues << "bar";
  QList<int> ids = Record::findIds("foo", "WHERE bar = ?", bindValues);
  QCOMPARE(ids.size(), 1);
  QCOMPARE(ids[0], 2);
}

void TestRecord::staticFindIdsWithEverything()
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

void TestRecord::staticCount()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  QCOMPARE(Record::count("foo"), 1);
}

void TestRecord::staticCountWithConditions()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar')");

  QCOMPARE(Record::count("foo", "WHERE bar = 'bar'"), 1);
}

void TestRecord::staticCountWithConditionsAndBindValues()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar')");

  QList<QVariant> bindValues;
  bindValues << "bar";
  QCOMPARE(Record::count("foo", "WHERE bar = ?", bindValues), 1);
}

void TestRecord::getDirty()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  m_records = Record::find<Record>("foo");
  m_records[0]->set("bar", "baz");
  QCOMPARE(m_records[0]->get("bar").toString(), QString("baz"));
}

void TestRecord::getOriginal()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  m_records = Record::find<Record>("foo");
  m_records[0]->set("bar", "baz");
  QCOMPARE(m_records[0]->get("bar", false).toString(), QString("foo"));
}

void TestRecord::isModifiedAfterSet()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  m_records = Record::find<Record>("foo");
  m_records[0]->set("bar", "baz");
  QVERIFY(m_records[0]->isModified());
}

void TestRecord::unsetOriginalValue()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  m_records = Record::find<Record>("foo");
  m_records[0]->unset("bar");
  QCOMPARE(m_records[0]->get("bar"), QVariant());
  QCOMPARE(m_records[0]->get("bar", false).toString(), QString("foo"));
}

void TestRecord::unsetDirtyValue()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  m_records = Record::find<Record>("foo");
  m_records[0]->set("bar", "baz");
  m_records[0]->unset("bar");
  QCOMPARE(m_records[0]->get("bar").toString(), QString("foo"));
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
