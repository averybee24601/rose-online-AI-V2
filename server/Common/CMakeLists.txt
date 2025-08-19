add_library(rose_common STATIC
  SocketBase.cpp
  SocketClient.cpp
  SocketServer.cpp
  config.cpp
  log.cpp
  rosecrypt.cpp
  database/database.cpp
)

# Public headers include directory for consumers
target_include_directories(rose_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link pthreads
target_link_libraries(rose_common PUBLIC Threads::Threads)

# Link MySQL client lib if found
if(MYSQL_CLIENT_LIB)
  target_link_libraries(rose_common PUBLIC ${MYSQL_CLIENT_LIB})
endif()
