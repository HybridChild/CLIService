set(SOURCES
  CLIService.cpp
  CommandMenuTree.cpp
  CommandRequest.cpp
  MenuNode.cpp
)

add_library(${PROJECT_NAME}_lib STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}_lib
  PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
