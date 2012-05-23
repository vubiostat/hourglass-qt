FIND_PACKAGE(Qt4 REQUIRED)

SET(hourglass_QT_SOURCES browser.cpp launcher.cpp main.cpp popup.cpp thread.cpp)
SET(hourglass_QT_HEADERS browser.h launcher.h popup.h thread.h)
SET(hourglass_SOURCES ${hourglass_QT_SOURCES} mongoose.c server.cpp)

SET(QT_USE_QTWEBKIT TRUE)
SET(QT_USE_QTSQL TRUE)
QT4_WRAP_CPP(hourglass_QT_HEADERS_MOC ${hourglass_QT_HEADERS})
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})
SET(hourglass_HEADERS ${hourglass_QT_HEADERS_MOC} mongoose.h server.h)

ADD_EXECUTABLE(hourglass ${hourglass_SOURCES} ${hourglass_HEADERS})
TARGET_LINK_LIBRARIES(hourglass ${QT_LIBRARIES})
