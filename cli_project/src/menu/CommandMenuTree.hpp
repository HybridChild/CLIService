#pragma once

#include "../command/CommandRequest.hpp"
#include "MenuNode.hpp"

class CommandMenuTree {
public:
  CommandMenuTree();

  MenuNode* getRoot();
  MenuNode* getCurrentNode() { return _currentNode; }
  void setCurrentNode(MenuNode* node) { _currentNode = node; }
  std::string getPath(const MenuNode* node) const;
  bool navigateToNode(const std::vector<std::string>& path, bool absolute = false);
  void returnToRoot() { _currentNode = &_root; }

private:
  MenuNode _root;
  MenuNode* _currentNode;
};
