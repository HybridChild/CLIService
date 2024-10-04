#pragma once

#include "../command/CommandRequest.hpp"
#include "MenuNode.hpp"

class CommandMenuTree {
public:
  CommandMenuTree();

  MenuNode* getCurrentNode();
  void setCurrentNode(MenuNode* node);
  MenuNode* getRoot();

  void processRequest(const CommandRequest& request, std::string& response);
  std::string getCurrentPath() const;

private:
  MenuNode _root;
  MenuNode* _currentNode;

  bool navigate(const std::vector<std::string>& path, bool isAbsolute);
  bool executeCommand(const CommandRequest& request, std::string& response);
};
