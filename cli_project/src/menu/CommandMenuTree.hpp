#pragma once

#include "../command/CommandRequest.hpp"
#include "MenuNode.hpp"

class CommandMenuTree {
public:
  CommandMenuTree();

  MenuNode* getCurrentNode();
  void setCurrentNode(MenuNode* node) { _currentNode = node; }
  MenuNode* getRoot();

  CommandRequest processRequest(const CommandRequest& request);
  std::string getCurrentPath() const;

private:
  MenuNode _root;
  MenuNode* _currentNode;

  bool navigate(const std::vector<std::string>& path, bool isAbsolute);
  bool executeCommand(CommandRequest& request);
};
