#include "test_record.h"

const QString RecordSub::s_tableName = QString("foo");

RecordSub::RecordSub(QObject *parent)
  : Record(parent)
{
}

RecordSub::RecordSub(const QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent)
  : Record(attributes, newRecord, parent)
{
}

const QString &RecordSub::classTableName()
{
  return s_tableName;
}

const QString &RecordSub::tableName() const
{
  return s_tableName;
}

RecordSpy::RecordSpy(QObject *parent)
  : RecordSub(parent)
{
  initializeValues();
}

RecordSpy::RecordSpy(const QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent)
  : RecordSub(attributes, newRecord, parent)
{
  initializeValues();
}

void RecordSpy::initializeValues()
{
  m_wasNewAfterCreate = false;
  m_wasNewAfterSave = false;
  m_beforeValidationCalled = false;
  m_beforeCreateCalled = false;
  m_beforeSaveCalled = false;
}

bool RecordSpy::wasNewAfterCreate()
{
  return m_wasNewAfterCreate;
}

bool RecordSpy::wasNewAfterSave()
{
  return m_wasNewAfterSave;
}

bool RecordSpy::beforeValidationCalled()
{
  return m_beforeValidationCalled;
}

bool RecordSpy::beforeCreateCalled()
{
  return m_beforeCreateCalled;
}

bool RecordSpy::beforeSaveCalled()
{
  return m_beforeSaveCalled;
}

void RecordSpy::beforeCreate()
{
  m_beforeCreateCalled = true;
}

void RecordSpy::beforeSave()
{
  m_beforeSaveCalled = true;
}

void RecordSpy::afterCreate()
{
  m_wasNewAfterCreate = wasNew();
}

void RecordSpy::afterSave()
{
  m_wasNewAfterSave = wasNew();
}

void RecordSpy::beforeValidation()
{
  m_beforeValidationCalled = true;
}

RecordMock::RecordMock(QObject *parent)
  : RecordSub(parent), m_validateReturnValue(true)
{
}

RecordMock::RecordMock(const QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent)
  : RecordSub(attributes, newRecord, parent)
{
}

void RecordMock::setValidateReturnValue(bool value)
{
  m_validateReturnValue = value;
}

bool RecordMock::validate()
{
  return m_validateReturnValue;
}

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
  while (!m_record_spies.isEmpty()) {
    delete m_record_spies.takeLast();
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

  m_records = Record::find<RecordSub>();
  QCOMPARE(m_records.size(), 1);
  QCOMPARE(m_records[0]->id(), 1);
  QCOMPARE(m_records[0]->get("bar").toString(), QString("foo"));
}

void TestRecord::staticFindWithParent()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  m_records = Record::find<RecordSub>(this);
  QCOMPARE(m_records.size(), 1);
  QCOMPARE(m_records[0]->parent(), this);
}

void TestRecord::staticFindWithConditions()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar')");

  m_records = Record::find<RecordSub>("WHERE bar = 'foo'");
  QCOMPARE(m_records.size(), 1);
  QCOMPARE(m_records[0]->id(), 1);
  QCOMPARE(m_records[0]->get("bar").toString(), QString("foo"));
}

void TestRecord::staticFindWithConditionsAndPredicate()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar 2'), ('bar 1')");

  m_records = Record::find<RecordSub>("WHERE bar LIKE 'bar%'", "ORDER BY bar");
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
  m_records = Record::find<RecordSub>("WHERE bar = ?", bindValues);
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
  m_records = Record::find<RecordSub>("WHERE bar LIKE ?", bindValues, "ORDER BY bar");
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

  QList<int> ids = Record::findIds<RecordSub>();
  QCOMPARE(ids.size(), 1);
  QCOMPARE(ids[0], 1);
}

void TestRecord::staticFindIdsWithConditions()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar')");

  QList<int> ids = Record::findIds<RecordSub>("WHERE bar = 'bar'");
  QCOMPARE(ids.size(), 1);
  QCOMPARE(ids[0], 2);
}

void TestRecord::staticFindIdsWithConditionsAndPredicate()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar 2'), ('bar 1')");

  QList<int> ids = Record::findIds<RecordSub>("WHERE bar LIKE 'bar%'", "ORDER BY bar");
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
  QList<int> ids = Record::findIds<RecordSub>("WHERE bar = ?", bindValues);
  QCOMPARE(ids.size(), 1);
  QCOMPARE(ids[0], 2);
}

void TestRecord::staticFindIdsWithEverything()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar 2'), ('bar 1')");

  QList<QVariant> bindValues;
  bindValues << "bar%";
  QList<int> ids = Record::findIds<RecordSub>("WHERE bar LIKE ?", bindValues, "ORDER BY bar");
  QCOMPARE(ids.size(), 2);
  QCOMPARE(ids[0], 3);
  QCOMPARE(ids[1], 2);
}

void TestRecord::staticCount()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  QCOMPARE(Record::count<RecordSub>(), 1);
}

void TestRecord::staticCountWithConditions()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar')");

  QCOMPARE(Record::count<RecordSub>("WHERE bar = 'bar'"), 1);
}

void TestRecord::staticCountWithConditionsAndBindValues()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo'), ('bar')");

  QList<QVariant> bindValues;
  bindValues << "bar";
  QCOMPARE(Record::count<RecordSub>("WHERE bar = ?", bindValues), 1);
}

void TestRecord::getDirty()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  m_records = Record::find<RecordSub>();
  m_records[0]->set("bar", "baz");
  QCOMPARE(m_records[0]->get("bar").toString(), QString("baz"));
}

void TestRecord::getOriginal()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  m_records = Record::find<RecordSub>();
  m_records[0]->set("bar", "baz");
  QCOMPARE(m_records[0]->get("bar", false).toString(), QString("foo"));
}

void TestRecord::unsetOriginalValue()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  m_records = Record::find<RecordSub>();
  m_records[0]->unset("bar");
  QCOMPARE(m_records[0]->get("bar"), QVariant());
  QCOMPARE(m_records[0]->get("bar", false).toString(), QString("foo"));
}

void TestRecord::unsetDirtyValue()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  m_records = Record::find<RecordSub>();
  m_records[0]->set("bar", "baz");
  m_records[0]->unset("bar");
  QCOMPARE(m_records[0]->get("bar").toString(), QString("foo"));
}

void TestRecord::isNew()
{
  RecordSub record;
  QVERIFY(record.isNew());
}

void TestRecord::wasNew()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT, created_at TEXT, updated_at TEXT)");

  RecordSpy spy;
  spy.set("bar", "foo");
  spy.save();
  QVERIFY(spy.wasNewAfterCreate());
  QVERIFY(spy.wasNewAfterSave());
  QVERIFY(!spy.wasNew());
}

void TestRecord::isModifiedInitialValue()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  /* New record */
  RecordSub record;
  QVERIFY(!record.isModified());

  /* Existing record */
  m_records = Record::find<RecordSub>();
  QVERIFY(!m_records[0]->isModified());
}

void TestRecord::isModifiedAfterSet()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  /* New record */
  RecordSub record;
  record.set("bar", "baz");
  QVERIFY(record.isModified());

  /* Existing record */
  m_records = Record::find<RecordSub>();
  m_records[0]->set("bar", "baz");
  QVERIFY(m_records[0]->isModified());
}

void TestRecord::isModifiedAfterUnset()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  m_records = Record::find<RecordSub>();
  m_records[0]->unset("bar");
  QVERIFY(m_records[0]->isModified());
}

void TestRecord::isValidDependsOnValidate()
{
  RecordMock mock;
  mock.setValidateReturnValue(true);
  QVERIFY(mock.isValid());

  mock.setValidateReturnValue(false);
  QVERIFY(!mock.isValid());
}

void TestRecord::isValidCallsBeforeValidation()
{
  RecordSpy spy;
  spy.isValid();
  QVERIFY(spy.beforeValidationCalled());
}

void TestRecord::saveNewRecord()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT, created_at TEXT, updated_at TEXT)");

  RecordSub record;
  QSignalSpy spy(&record, SIGNAL(saved()));
  record.set("bar", "baz");
  record.save();
  QCOMPARE(record.id(), 1);
  QCOMPARE(spy.count(), 1);

  m_records = Record::find<RecordSub>("WHERE id = 1");
  QCOMPARE(m_records[0]->id(), 1);
}

void TestRecord::saveReturnsFalseIfInvalid()
{
  RecordMock mock;
  mock.setValidateReturnValue(false);
  QVERIFY(!mock.save());
}

void TestRecord::saveCallsBeforeCreateForNewRecordsOnly()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT, created_at TEXT, updated_at TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  /* New record */
  RecordSpy spy;
  spy.set("bar", "baz");
  spy.save();
  QVERIFY(spy.beforeCreateCalled());

  /* Existing record */
  m_record_spies = Record::find<RecordSpy>();
  m_record_spies[0]->set("bar", "baz");
  m_record_spies[0]->save();
  QVERIFY(!m_record_spies[0]->beforeCreateCalled());
}

void TestRecord::saveCallsBeforeSave()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT, created_at TEXT, updated_at TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  /* New record */
  RecordSpy spy;
  spy.set("bar", "baz");
  spy.save();
  QVERIFY(spy.beforeSaveCalled());

  /* Existing record */
  m_record_spies = Record::find<RecordSpy>();
  m_record_spies[0]->set("bar", "baz");
  m_record_spies[0]->save();
  QVERIFY(m_record_spies[0]->beforeSaveCalled());
}

void TestRecord::destroyNewRecordReturnsFalse()
{
  RecordSub record;
  QVERIFY(!record.destroy());
}

void TestRecord::destroyExistingRecord()
{
  executeQuery("CREATE TABLE foo (id INTEGER PRIMARY KEY AUTOINCREMENT, bar TEXT, created_at TEXT, updated_at TEXT)");
  executeQuery("INSERT INTO foo (bar) VALUES ('foo')");

  m_records = Record::find<RecordSub>();
  QSignalSpy spy(m_records[0], SIGNAL(destroyed()));
  m_records[0]->destroy();

  QCOMPARE(spy.count(), 1);
  QCOMPARE(Record::count<RecordSub>(), 0);
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
