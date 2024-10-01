#pragma once

#include "MenuNode.hpp"
#include "../command/CommandRequest.hpp"

class CommandMenuTree {
public:
  CommandMenuTree();

  MenuNode* getCurrentNode();
  MenuNode* getRoot();

  void processRequest(const CommandRequest& request);
  std::string getCurrentPath();

private:
  MenuNode root;
  MenuNode* currentNode;

  void navigate(const std::vector<std::string>& path, bool isAbsolute);
  bool executeCommand(CommandRequest& request);
};
