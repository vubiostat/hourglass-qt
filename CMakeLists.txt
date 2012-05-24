CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
INCLUDE(ExternalProject)
SET(CMAKE_VERBOSE_MAKEFILE true)

PROJECT(hourglass)

ExternalProject_Add(teng2.1.1
  URL "http://prdownloads.sourceforge.net/teng/teng-2.1.1.tar.gz"
  CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>)
ExternalProject_Get_Property(teng2.1.1 INSTALL_DIR)
SET(TENG_INSTALL_DIR ${INSTALL_DIR})
SET(INSTALL_DIR)

SET(hourglass_PUBLIC_PATH ${CMAKE_CURRENT_BINARY_DIR}/public)
SET(hourglass_VIEW_PATH ${CMAKE_CURRENT_BINARY_DIR}/views)

ADD_CUSTOM_TARGET(public
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/public ${hourglass_PUBLIC_PATH})

ADD_CUSTOM_TARGET(views
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/views ${hourglass_VIEW_PATH})

ADD_SUBDIRECTORY(src)
ADD_DEPENDENCIES(hourglass teng2.1.1 public views)
