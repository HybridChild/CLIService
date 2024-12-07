set(SOURCES
  src/cli/ActionRequest.cpp
  src/cli/CLIService.cpp
  src/cli/InputParser.cpp
  src/cli/LoginRequest.cpp
  src/cli/User.cpp
  src/tree/Directory.cpp
  src/tree/Path.cpp
  src/tree/PathResolver.cpp
)

add_library(${PROJECT_NAME}_lib
  ${SOURCES}
)

target_include_directories(${PROJECT_NAME}_lib
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
