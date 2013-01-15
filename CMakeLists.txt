cmake_minimum_required (VERSION 2.6)

# Project Name
project (scom)

# Add MySQL
# find_package(MySQL REQUIRED)
include_directories(/inc /usr/include/mysql )
set(CMAKE_LIBRARY_PATH /usr/lib/mysql /lib )
set(LIBS ${LIBS} mysqlclient)

# Include Directory
include_directories(src)

# Link the MySQL
link_directories("${PROJECT_SOURCE_DIR}/lib") 


# Add Sources
add_executable( mtbus 
	src/main.c 
	src/MTBus.c 
	src/UartManager.c 
	src/CHS.c 
	src/connect.c 
	src/MTBusConfiguration.c 
	src/MTBusManager.c 
	src/MTbusServer.c 
	src/MTBusRoutingTable.c 
	src/Terabus.c 
	src/NetVarAccesManager.c 
	src/TBusManager.c 
	src/UartManager.c 
	src/UartDriver.c 
	src/netdev.c 
	src/archive.c
)

target_link_libraries(mtbus -L -i ${LIBS})
