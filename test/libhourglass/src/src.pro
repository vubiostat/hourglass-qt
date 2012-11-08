include(../../../src/libhourglass/libhourglass.pri)

TARGET = test_hourglass

TEMPLATE = app
DESTDIR = ../../../bin

QT += sql testlib

OBJECTS_DIR = .obj
MOC_DIR = .moc
RCC_DIR = .rcc
UI_DIR = .ui

SOURCES += main.cpp test_record.cpp
HEADERS += test_record.h
LIBS += -L../../../lib -lhourglass -Wl,-rpath,$$PWD/../../../lib
