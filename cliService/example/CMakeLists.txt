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

# Create a consistent example target name
set(EXAMPLE_TARGET "Example_CLI")
add_executable(${EXAMPLE_TARGET} ${EXAMPLE_SOURCES})

target_include_directories(${EXAMPLE_TARGET}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${EXAMPLE_TARGET}
  PRIVATE
    ${PROJECT_NAME}_lib
)

# Optionally set the output name if you want it to still be named CLIService_example
set_target_properties(${EXAMPLE_TARGET} PROPERTIES
  OUTPUT_NAME "${PROJECT_NAME}_example"
)
