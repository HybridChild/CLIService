#pragma once

#include "../command/CommandRequest.hpp"
#include "MenuNode.hpp"

class CommandMenuTree {
public:
  CommandMenuTree();

  MenuNode* getCurrentNode();
  MenuNode* getRoot();

  CommandRequest processRequest(const CommandRequest& request);
  std::string getCurrentPath();

private:
  MenuNode root;
  MenuNode* currentNode;

  bool navigate(const std::vector<std::string>& path, bool isAbsolute);
  bool executeCommand(CommandRequest& request);
};