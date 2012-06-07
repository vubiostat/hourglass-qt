CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE(ExternalProject)
#SET(CMAKE_VERBOSE_MAKEFILE true)

PROJECT(hourglass)
SET(CMAKE_CXX_FLAGS "-g3")

ExternalProject_Add(ctemplate-2.2
  URL "http://ctemplate.googlecode.com/files/ctemplate-2.2.tar.gz"
  URL_MD5 "1de89d9073f473c1e31862c4581636f3"
  CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>)
ExternalProject_Get_Property(ctemplate-2.2 INSTALL_DIR)
SET(CTEMPLATE_INSTALL_DIR ${INSTALL_DIR})
SET(INSTALL_DIR)

ExternalProject_Add(qhttpserver-git
  GIT_REPOSITORY "https://github.com/viking/qhttpserver"
  CONFIGURE_COMMAND cd <SOURCE_DIR> && ${QT_QMAKE_EXECUTABLE} -r PREFIX=<INSTALL_DIR>
  BUILD_COMMAND cd <SOURCE_DIR> && make
  INSTALL_COMMAND cd <SOURCE_DIR> && make install)
ExternalProject_Get_Property(qhttpserver-git INSTALL_DIR)
SET(QHTTPSERVER_INSTALL_DIR ${INSTALL_DIR})
SET(INSTALL_DIR)

ADD_SUBDIRECTORY(src)
ADD_DEPENDENCIES(hourglass ctemplate-2.2 qhttpserver-git)
