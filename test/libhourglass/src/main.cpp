#include <QtTest/QtTest>
#include "test_record.h"

int main(int argc, char *argv[])
{
  int retval = 0;
  QCoreApplication app(argc, argv);

  TestRecord testRecord;
  retval |= QTest::qExec(&testRecord, argc, argv);

  return retval;
}
