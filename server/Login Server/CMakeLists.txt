add_executable(login_server
  main.cpp
  loginserver.cpp
  loginpackets.cpp
  loginclient.cpp
  extrafunction.cpp
  signal.cpp
)

target_link_libraries(login_server PRIVATE rose_common)

# For Windows builds define _WIN32 when appropriate (CMake does it automatically based on generator)

install(TARGETS login_server RUNTIME DESTINATION bin)
