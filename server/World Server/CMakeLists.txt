# Build world_server by globbing all C++ sources in this folder and subfolders
file(GLOB WORLD_SOURCES
  "*.cpp"
  "aip/*.cpp"
  "quest/*.cpp"
)

add_executable(world_server ${WORLD_SOURCES})

target_include_directories(world_server PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/aip
  ${CMAKE_CURRENT_SOURCE_DIR}/quest
)

# Link against common library
target_link_libraries(world_server PRIVATE rose_common)

install(TARGETS world_server RUNTIME DESTINATION bin)
