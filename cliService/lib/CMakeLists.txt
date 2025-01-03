set(LIB_HEADERS
  include/cliService/cli/CommandRequest.hpp
  include/cliService/cli/CLIMessages.hpp
  include/cliService/cli/CLIService.hpp
  include/cliService/cli/CLIServiceConfiguration.hpp
  include/cliService/cli/CLIState.hpp
  include/cliService/cli/CommandHistory.hpp
  include/cliService/cli/InputParser.hpp
  include/cliService/cli/LoginRequest.hpp
  include/cliService/cli/RequestBase.hpp
  include/cliService/cli/CharIOStreamIf.hpp
  include/cliService/cli/TabCompletionRequest.hpp
  include/cliService/cli/User.hpp
  include/cliService/tree/CommandIf.hpp
  include/cliService/tree/CLIResponse.hpp
  include/cliService/tree/Directory.hpp
  include/cliService/tree/NodeIf.hpp
  include/cliService/tree/Path.hpp
  include/cliService/tree/PathCompleter.hpp
  include/cliService/tree/PathResolver.hpp
)

set(LIB_SOURCES
  src/cli/CLIService.cpp
  src/cli/InputParser.cpp
  src/tree/Directory.cpp
  src/tree/Path.cpp
  src/tree/PathResolver.cpp
)

add_library(${PROJECT_NAME}_lib
  ${LIB_HEADERS}
  ${LIB_SOURCES}
)

target_include_directories(${PROJECT_NAME}_lib
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
