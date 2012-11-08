include(../../../hourglass.pri)
include(../../libhourglass/libhourglass.pri)
include(ui/ui.pri)
include(icons/icons.pri)

TARGET = hourglass

TEMPLATE = app
DESTDIR = ../../../bin

QT += sql
CONFIG += debug
QMAKE_CXXFLAGS_DEBUG += -g3 -O0

OBJECTS_DIR = .obj
MOC_DIR = .moc
RCC_DIR = .rcc
UI_DIR = .ui

INCLUDEPATH += $$PWD

SOURCES += \
  aboutdialog.cpp \
  abstractlineedit.cpp \
  activitydelegate.cpp \
  activitydialog.cpp \
  activitylineedit.cpp \
  activitytableview.cpp \
  currentactivitydelegate.cpp \
  currentactivitytableview.cpp \
  hourglass.cpp \
  main.cpp \
  mainwindow.cpp \
  namesdelegate.cpp \
  preferencesdialog.cpp \
  projecttotaldelegate.cpp \
  projecttotalslistview.cpp \
  tagslineedit.cpp

HEADERS += \
  aboutdialog.h \
  abstractlineedit.h \
  activitydelegate.h \
  activitydialog.h \
  activitylineedit.h \
  activitytableview.h \
  currentactivitydelegate.h \
  currentactivitytableview.h \
  hourglass.h \
  mainwindow.h \
  namesdelegate.h \
  preferencesdialog.h \
  projecttotaldelegate.h \
  projecttotalslistview.h \
  tagslineedit.h \
  version.h

LIBS += -L../../../lib -lhourglass -Wl,-rpath,$$PWD/../../../lib

target.path = $${PREFIX}/bin
desktop.files += hourglass.desktop
desktop.path = $${PREFIX}/share/applications
icon.files += icons/hourglass.png
icon.path = $${PREFIX}/share/pixmaps
INSTALLS += target desktop icon
