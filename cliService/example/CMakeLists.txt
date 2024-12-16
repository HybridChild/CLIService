set(EXAMPLE_HEADERS
  commands/AccessLevel.hpp
  commands/hw/PotmeterGetCommand.hpp
  commands/hw/RgbLedSetCommand.hpp
  commands/hw/ToggleSwitchGetCommand.hpp
  commands/system/HeapStatsGetCommand.hpp
  commands/system/RebootCommand.hpp
  io/UnixWinCharIO.hpp
  util/util.hpp
)

set(EXAMPLE_SOURCES
  main.cpp
  io/UnixWinCharIOStream.cpp
)

add_executable(${PROJECT_NAME}_example ${EXAMPLE_SOURCES})

target_include_directories(${PROJECT_NAME}_example
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}_example
  PRIVATE
    ${PROJECT_NAME}_lib
)
