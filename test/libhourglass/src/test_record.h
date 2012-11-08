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

    void findIds();
    void findIdsWithConditions();
    void findIdsWithConditionsAndPredicate();
    void findIdsWithConditionsAndBindValues();
    void findIdsWithEverything();

  private:
    QSqlDatabase m_database;

    void executeQuery(const QString &query);
};

#endif
