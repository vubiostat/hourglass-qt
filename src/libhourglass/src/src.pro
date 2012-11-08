include(../libhourglass.pri)
include(../include/include.pri)

TARGET = hourglass

TEMPLATE = lib
DESTDIR = ../../../lib

QT += sql
CONFIG += dll

OBJECTS_DIR = .obj
MOC_DIR = .moc
RCC_DIR = .rcc
UI_DIR = .ui

SOURCES += abstractactivitymodel.cpp abstractnameslistmodel.cpp activity.cpp activitynameslistmodel.cpp activitytablemodel.cpp currentactivitytablemodel.cpp project.cpp projecttotalslistmodel.cpp record.cpp recordmanager.cpp tag.cpp tagnameslistmodel.cpp databasemanager.cpp
