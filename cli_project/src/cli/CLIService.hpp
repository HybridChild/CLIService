#pragma once

#include "../menu/CommandMenuTree.hpp"
#include <memory>
#include <string>

class CLIService {
public:
  CLIService(std::unique_ptr<CommandMenuTree> tree);

  void processCommand(const std::string& input);
  void listCurrentCommands();
  void run();

private:
  std::unique_ptr<CommandMenuTree> menuTree;
  void printResponse(const CommandRequest& request);
};