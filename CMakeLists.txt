project(TEST)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_FLAGS "-g -Wall $(mysql_config --cflags) $(mysql_config --libs)")
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(APP_NAME scom)

# DEBUG|RELEASE|RELWITHDEBINFO|MINSIZEREL
set(CMAKE_BUILD_TYPE DEBUG)

# - Find mysqlclient
# Find the native MySQL includes and library
#
#  MYSQL_INCLUDE_DIR - where to find mysql.h, etc.
#  MYSQL_LIBRARIES   - List of libraries when using MySQL.
#  MYSQL_FOUND       - True if MySQL found.

IF (MYSQL_INCLUDE_DIR)
  # Already in cache, be silent
  SET(MYSQL_FIND_QUIETLY TRUE)
ENDIF (MYSQL_INCLUDE_DIR)

FIND_PATH(MYSQL_INCLUDE_DIR mysql.h
	PATH_SUFFIXES "mysql"
)

SET(MYSQL_NAMES mysqlclient mysqlclient_r)
FIND_LIBRARY(MYSQL_LIBRARY
  NAMES ${MYSQL_NAMES}
  PATHS /usr/lib /usr/local/lib
  PATH_SUFFIXES mysql
)

IF (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
  SET(MYSQL_FOUND TRUE)
  SET( MYSQL_LIBRARIES ${MYSQL_LIBRARY} )
ELSE (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
  SET(MYSQL_FOUND FALSE)
  SET( MYSQL_LIBRARIES )
ENDIF (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)

IF (MYSQL_FOUND)
  IF (NOT MYSQL_FIND_QUIETLY)
    MESSAGE(STATUS "Found MySQL: ${MYSQL_LIBRARY}")
    FILE(STRINGS "${MYSQL_INCLUDE_DIR}/mysql_version.h" MYSQL_VERSION_H REGEX "^#define[ \t]+MYSQL_SERVER_VERSION[ \t]+\"[^\"]+\".*$" )
	string( REGEX REPLACE "^.*MYSQL_SERVER_VERSION[ \t]+\"([^\"]+)\".*$" "\\1" MYSQL_VERSION_STRING "${MYSQL_VERSION_H}" )    
  ENDIF (NOT MYSQL_FIND_QUIETLY)
ELSE (MYSQL_FOUND)
  IF (MYSQL_FIND_REQUIRED)
    MESSAGE(STATUS "Looked for MySQL libraries named ${MYSQL_NAMES}.")
    MESSAGE(FATAL_ERROR "Could NOT find MySQL library")
  ENDIF (MYSQL_FIND_REQUIRED)
ENDIF (MYSQL_FOUND)

MARK_AS_ADVANCED(
  MYSQL_LIBRARY
  MYSQL_INCLUDE_DIR
  )

include_directories( /inc /MYSQL_INCLUDE_DIR)
#include_directories( /usr/include )
link_directories( /bin )
#link_directories( /usr/lib )

list(APPEND SRC ${SRC_DIR}/main.c)
list(APPEND SRC ${SRC_DIR}/connect.c)
list(APPEND SRC ${SRC_DIR}/UartManager.c)
list(APPEND SRC ${SRC_DIR}/UartDriver.c)
list(APPEND SRC ${SRC_DIR}/Terabus.c)
list(APPEND SRC ${SRC_DIR}/TBusManager.c)
list(APPEND SRC ${SRC_DIR}/NetVarAccesManager.c)
list(APPEND SRC ${SRC_DIR}/MTbusServer.c)
list(APPEND SRC ${SRC_DIR}/MTBusRoutingTable.c)
list(APPEND SRC ${SRC_DIR}/MTBusManager.c)
list(APPEND SRC ${SRC_DIR}/MTBusConfiguration.c)
list(APPEND SRC ${SRC_DIR}/MTBus.c)
list(APPEND SRC ${SRC_DIR}/CHS.c)
list(APPEND SRC ${SRC_DIR}/netdev.c)
list(APPEND SRC ${SRC_DIR}/archive.c)


add_executable( ${APP_NAME} ${SRC} )

#target_link_libraries( ${APP_NAME} name_N)
