cmake_minimum_required(VERSION 3.16)
project(rose_evolved_servers LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Add local include path for bundled MySQL headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Threads
find_package(Threads REQUIRED)

# Try to find system MariaDB/MySQL client library
find_path(MYSQL_INCLUDE_DIR mysql.h)
find_library(MYSQL_CLIENT_LIB NAMES mariadb mysqlclient libmysql libmariadb)

if(NOT MYSQL_CLIENT_LIB)
  message(WARNING "No system MariaDB/MySQL client library found.\nYou can install libmariadb-dev (Debian/Ubuntu) or mariadb-connector-c.\nCMake will still generate build files, but linking will fail until the library is available.")
endif()

add_subdirectory(server/Common)
add_subdirectory(server/Login Server)
add_subdirectory(server/Char Server)
add_subdirectory(server/World Server)
