project(TEST)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_FLAGS "-g -Wall")
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(APP_NAME prog)

# DEBUG|RELEASE|RELWITHDEBINFO|MINSIZEREL
set(CMAKE_BUILD_TYPE RELEASE)

include_directories( /inc )
link_directories( /bin )

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


add_executable( ${APP_NAME} ${SRC} )

#target_link_libraries( ${APP_NAME} name_N)
