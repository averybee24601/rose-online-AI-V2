add_executable(char_server
  main.cpp
  charserver.cpp
  charpackets.cpp
  charclient.cpp
  community.cpp
  clan.cpp
  extrafunctions.cpp
  signal.cpp
)

target_link_libraries(char_server PRIVATE rose_common)

install(TARGETS char_server RUNTIME DESTINATION bin)
