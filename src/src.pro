include(../hourglass.pri)
include(ui/ui.pri)
#include(public/public.pri)
#include(views/views.pri)
#include(icons/icons.pri)

TARGET = hourglass

TEMPLATE = app
DESTDIR = ../bin

QT += sql
CONFIG += debug
QMAKE_CXXFLAGS_DEBUG += -g3 -O0

OBJECTS_DIR = .obj
MOC_DIR = .moc
RCC_DIR = .rcc
UI_DIR = .ui

SOURCES += mainwindow.cpp hourglass.cpp activitytablemodel.cpp activitydelegate.cpp activitytableview.cpp activitylineedit.cpp activitydialog.cpp record.cpp project.cpp tag.cpp activity.cpp main.cpp
HEADERS += mainwindow.h hourglass.h activitytablemodel.h activitydelegate.h activitytableview.h activitylineedit.h activitydialog.h record.h project.h tag.h activity.h
#SOURCES += browser.cpp window.cpp main.cpp popup.cpp thread.cpp server.cpp database.cpp activity.cpp project.cpp tag.cpp view.cpp model.cpp hourglass.cpp dictionary.cpp controller.cpp setting.cpp about.cpp
#HEADERS += browser.h window.h popup.h thread.h database.h activity.h project.h tag.h model.h server.h view.h dictionary.h hourglass.h controller.h setting.h about.h
#LIBS += -lctemplate -lqhttpserver

target.path = $${PREFIX}/bin
INSTALLS += target
