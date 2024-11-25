add_executable(cli_app
  main.cpp
  cli/CLIService.cpp
  menu/CommandRequest.cpp
  commands/RgbLedCommand.cpp
  commands/PotmeterCommand.cpp
  commands/GetAnalyticsCommand.cpp
  menu/MenuNode.cpp
  menu/CommandMenuTree.cpp
  factory/CommandMenuTreeFactory.cpp
  io/IOStreamIf.hpp
  io/StdIOStream.hpp
)

target_include_directories(cli_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
