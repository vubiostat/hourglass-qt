FIND_PACKAGE(Qt4 COMPONENTS QtCore QtSql QtNetwork QtGui REQUIRED)
SET(QT_USE_QTWEBKIT TRUE)
SET(QT_USE_QTSQL TRUE)

SET(hourglass_QT_SOURCES
  browser.cpp
  launcher.cpp
  main.cpp
  popup.cpp
  thread.cpp
  server.cpp
  database.cpp
  activity.cpp
  project.cpp
  tag.cpp
  view.cpp
  model.cpp
  hourglass.cpp
  dictionary.cpp
  controller.cpp
)

SET(hourglass_QT_HEADERS
  browser.h
  launcher.h
  popup.h
  thread.h
  database.h
  activity.h
  project.h
  tag.h
  model.h
  server.h
  view.h
  dictionary.h
  hourglass.h
  controller.h
)

SET(hourglass_SOURCES ${hourglass_QT_SOURCES})

SET(hourglass_RCS
  ${CMAKE_CURRENT_SOURCE_DIR}/../public/public.rcs
  ${CMAKE_CURRENT_SOURCE_DIR}/../views/views.rcs
)

QT4_WRAP_CPP(hourglass_QT_HEADERS_MOC ${hourglass_QT_HEADERS})
QT4_ADD_RESOURCES(hourglass_RC_SRCS ${hourglass_RCS})
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})
SET(hourglass_HEADERS ${hourglass_QT_HEADERS_MOC})

INCLUDE_DIRECTORIES(${LIBCTEMPLATE_INCLUDE_DIR} ${LIBQHTTPSERVER_INCLUDE_DIR})
LINK_DIRECTORIES(${CTEMPLATE_INSTALL_DIR}/lib ${QHTTPSERVER_INSTALL_DIR}/lib)

ADD_EXECUTABLE(hourglass ${hourglass_SOURCES} ${hourglass_HEADERS} ${hourglass_RC_SRCS})
TARGET_LINK_LIBRARIES(hourglass ${QT_LIBRARIES} ${LIBCTEMPLATE_LIBRARY} ${LIBQHTTPSERVER_LIBRARY})

INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/hourglass DESTINATION bin)
