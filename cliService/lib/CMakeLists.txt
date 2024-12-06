set(SOURCES
  src/cli/CLIService.cpp
  src/parser/CommandParser.cpp
  src/path/Path.cpp
  src/path/PathResolver.cpp
  src/requests/ActionRequest.cpp
  src/requests/LoginRequest.cpp
  src/tree/Directory.cpp
  src/user/User.cpp
)

add_library(${PROJECT_NAME}_lib
  ${SOURCES}
)

target_include_directories(${PROJECT_NAME}_lib
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
