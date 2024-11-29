#include "cliService/CommandMenuTree.hpp"

namespace cliService {

  enum class AccessLevel;

  CommandMenuTree::CommandMenuTree()
    : _root("root", static_cast<AccessLevel>(0)), _currentNode(&_root)
  {}

  MenuNode* CommandMenuTree::getRoot() {
    return &_root;
  }

  MenuNode* CommandMenuTree::getCurrentNode() const {
    return _currentNode;
  }

  void CommandMenuTree::setCurrentNode(MenuNode* node) {
    _currentNode = node;
  }

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

  bool CommandMenuTree::navigateToNode(const std::vector<std::string>& path, bool absolute) {
    MenuNode* originalNode = _currentNode;

    if (absolute) {
      _currentNode = &_root;
    }

    for (const auto& pathSegment : path) {
      if (pathSegment == "..") {
        MenuNode* parent = _currentNode->getParent();

        if (parent) {
          _currentNode = parent;
        } else {
          _currentNode = originalNode;
          return false;
        }
      }
      else {
        MenuNode* nextNode = _currentNode->getSubMenu(pathSegment);
        
        if (!nextNode) {
          _currentNode = originalNode;
          return false;
        }
        else {
          _currentNode = nextNode;
        }
      }
    }

    return true;
  }

}
