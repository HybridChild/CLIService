add_executable(cli_app
  main.cpp
  cli/CLIService.cpp
  command/CommandRequest.cpp
  command/RgbLedCommand.cpp
  command/PotmeterCommand.cpp
  menu/MenuNode.cpp
  menu/CommandMenuTree.cpp
  menu/CommandMenuTreeFactory.cpp
)

target_include_directories(cli_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
