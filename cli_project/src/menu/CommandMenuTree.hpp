#pragma once

#include "CommandRequest.hpp"
#include "MenuNode.hpp"

class CommandMenuTree {
public:
  CommandMenuTree();

  MenuNode* getRoot();
  MenuNode* getCurrentNode() const;
  void setCurrentNode(MenuNode* node);
  std::string getPath(const MenuNode* node) const;
  bool navigateToNode(const std::vector<std::string>& path, bool absolute = false);
  void returnToRoot() { _currentNode = &_root; }

private:
  MenuNode _root;
  MenuNode* _currentNode;
};
