#include "CommandMenuTree.hpp"

CommandMenuTree::CommandMenuTree()
  : _root("root")
{}

MenuNode* CommandMenuTree::getRoot() { return &_root; }

std::string CommandMenuTree::getPath(const MenuNode* node) const {
  std::vector<std::string> path;

  while (node != &_root) {
    path.push_back(node->getName());
    node = node->getParent();
  }

  std::string result = "/";

  for (auto it = path.rbegin(); it != path.rend(); ++it) {
    result += *it + "/";
  }

  return result;
}
