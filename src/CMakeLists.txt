FIND_PACKAGE(Qt4 REQUIRED)

SET(hourglass_HOURGLASS_H ${CMAKE_CURRENT_BINARY_DIR}/hourglass.h)
CONFIGURE_FILE("hourglass.h.in" ${hourglass_HOURGLASS_H})

SET(hourglass_QT_SOURCES browser.cpp launcher.cpp main.cpp popup.cpp thread.cpp server.cpp database.cpp activity.cpp project.cpp tag.cpp view.cpp model.cpp hourglass.cpp dictionary.cpp)
SET(hourglass_QT_HEADERS browser.h launcher.h popup.h thread.h database.h activity.h project.h tag.h model.h server.h view.h dictionary.h ${hourglass_HOURGLASS_H})
SET(hourglass_SOURCES ${hourglass_QT_SOURCES} mongoose.c)

SET(QT_USE_QTWEBKIT TRUE)
SET(QT_USE_QTSQL TRUE)
QT4_WRAP_CPP(hourglass_QT_HEADERS_MOC ${hourglass_QT_HEADERS})
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})
SET(hourglass_HEADERS ${hourglass_QT_HEADERS_MOC} mongoose.h)

INCLUDE_DIRECTORIES(${CTEMPLATE_INSTALL_DIR}/include ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
LINK_DIRECTORIES(${CTEMPLATE_INSTALL_DIR}/lib)

ADD_EXECUTABLE(hourglass ${hourglass_SOURCES} ${hourglass_HEADERS})
TARGET_LINK_LIBRARIES(hourglass ${QT_LIBRARIES} ctemplate)
