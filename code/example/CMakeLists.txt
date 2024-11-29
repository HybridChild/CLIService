set(EXAMPLE_SOURCES
  main.cpp
  commands/GetAnalyticsCommand.cpp
  commands/PotmeterCommand.cpp
  commands/RgbLedCommand.cpp
  factory/CommandMenuTreeFactory.cpp
  io/StdIOStream.hpp
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
