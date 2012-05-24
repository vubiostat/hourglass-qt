FIND_PACKAGE(Qt4 REQUIRED)

SET(hourglass_VIEW_H ${CMAKE_CURRENT_BINARY_DIR}/view.h)
CONFIGURE_FILE("view.h.in" ${hourglass_VIEW_H})

SET(hourglass_QT_SOURCES browser.cpp launcher.cpp main.cpp popup.cpp thread.cpp server.cpp database.cpp activity.cpp view.cpp)
SET(hourglass_QT_HEADERS browser.h launcher.h popup.h thread.h server.h database.h activity.h ${hourglass_VIEW_H})
SET(hourglass_SOURCES ${hourglass_QT_SOURCES} mongoose.c server.cpp)

SET(QT_USE_QTWEBKIT TRUE)
SET(QT_USE_QTSQL TRUE)
QT4_WRAP_CPP(hourglass_QT_HEADERS_MOC ${hourglass_QT_HEADERS})
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})
SET(hourglass_HEADERS ${hourglass_QT_HEADERS_MOC} mongoose.h server.h)

INCLUDE_DIRECTORIES(${TENG_INSTALL_DIR}/include ${CMAKE_CURRENT_BINARY_DIR})
LINK_DIRECTORIES(${TENG_INSTALL_DIR}/lib)

ADD_EXECUTABLE(hourglass ${hourglass_SOURCES} ${hourglass_HEADERS})
TARGET_LINK_LIBRARIES(hourglass ${QT_LIBRARIES} teng)
