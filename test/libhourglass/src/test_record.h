#ifndef __TEST_RECORD_H
#define __TEST_RECORD_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtTest/QtTest>
#include "hourglass/record.h"

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

    void isModifiedAfterSet();

    void unsetOriginalValue();
    void unsetDirtyValue();

  private:
    QSqlDatabase m_database;
    QList<Record *> m_records;

    void executeQuery(const QString &query);
};

#endif
