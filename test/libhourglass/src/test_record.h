#ifndef __TEST_RECORD_H
#define __TEST_RECORD_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtTest/QtTest>
#include "hourglass/record.h"

class RecordSub : public Record
{
  public:
    static const QString &classTableName();

    RecordSub(QObject *parent = 0);
    RecordSub(const QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent = 0);

    const QString &tableName() const;

  private:
    static const QString s_tableName;
};

class RecordSpy : public RecordSub
{
  Q_OBJECT

  public:
    RecordSpy(QObject *parent = 0);
    RecordSpy(const QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent = 0);

    bool wasNewAfterCreate();
    bool wasNewAfterSave();
    bool beforeValidationCalled();
    bool beforeCreateCalled();
    bool beforeSaveCalled();

  protected:
    void beforeCreate();
    void beforeSave();
    void afterCreate();
    void afterSave();
    void beforeValidation();

  private:
    bool m_wasNewAfterCreate;
    bool m_wasNewAfterSave;
    bool m_beforeValidationCalled;
    bool m_beforeCreateCalled;
    bool m_beforeSaveCalled;

    void initializeValues();
};

class RecordMock : public RecordSub
{
  Q_OBJECT

  public:
    RecordMock(QObject *parent = 0);
    RecordMock(const QMap<QString, QVariant> &attributes, bool newRecord, QObject *parent = 0);

    void setValidateReturnValue(bool value);

  protected:
    bool validate();

  private:
    bool m_validateReturnValue;
};

class TestRecord : public QObject
{
  Q_OBJECT

  private slots:
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    void staticFind();
    void staticFindWithParent();
    void staticFindWithConditions();
    void staticFindWithConditionsAndPredicate();
    void staticFindWithConditionsAndBindValues();
    void staticFindWithEverything();

    void staticFindIds();
    void staticFindIdsWithConditions();
    void staticFindIdsWithConditionsAndPredicate();
    void staticFindIdsWithConditionsAndBindValues();
    void staticFindIdsWithEverything();

    void staticCount();
    void staticCountWithConditions();
    void staticCountWithConditionsAndBindValues();

    void getDirty();
    void getOriginal();

    void unsetOriginalValue();
    void unsetDirtyValue();

    void isNew();
    void wasNew();

    void isModifiedInitialValue();
    void isModifiedAfterSet();
    void isModifiedAfterUnset();

    void isValidDependsOnValidate();
    void isValidCallsBeforeValidation();

    void saveNewRecord();
    void saveReturnsFalseIfInvalid();
    void saveCallsBeforeCreateForNewRecordsOnly();
    void saveCallsBeforeSave();

    void destroyNewRecordReturnsFalse();
    void destroyExistingRecord();

  private:
    QSqlDatabase m_database;
    QList<RecordSub *> m_records;
    QList<RecordSpy *> m_record_spies;

    void executeQuery(const QString &query);
};

#endif
