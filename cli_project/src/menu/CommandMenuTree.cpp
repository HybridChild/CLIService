#include "CommandMenuTree.hpp"

CommandMenuTree::CommandMenuTree()
  : _root("root"), _currentNode(&_root)
{}

MenuNode* CommandMenuTree::getCurrentNode() { return _currentNode; }
void CommandMenuTree::setCurrentNode(MenuNode* node)  { _currentNode = node; }
MenuNode* CommandMenuTree::getRoot() { return &_root; }

void CommandMenuTree::processRequest(const CommandRequest& request, std::string& response) {
  switch (request.getType()) {
    case CommandRequest::Type::RootNavigation:
      _currentNode = &_root;
      break;
    case CommandRequest::Type::Navigation:
      if (!navigate(request.getPath(), request.isAbsolute())) {
        response = "Navigation failed: Invalid path\n";
      }
      break;
    case CommandRequest::Type::Execution:
      if (executeCommand(request, response)) {
        // Response is set by the command itself
      } else {
        response = "Unknown command. Use 'help' for available commands.\n";
      }
      break;
  }
}

std::string CommandMenuTree::getCurrentPath() const {
  std::vector<std::string> path;
  MenuNode* node = _currentNode;
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

bool CommandMenuTree::navigate(const std::vector<std::string>& path, bool isAbsolute) {
  MenuNode* node = isAbsolute ? &_root : _currentNode;

  for (const auto& segment : path) {
    if (segment == "..") {
      if (node != &_root && node->getParent() != nullptr) {
        node = node->getParent();
      }
    } else if (!segment.empty()) {
      MenuNode* next = node->getSubMenu(segment);
      if (next) {
        node = next;
      } else {
        return false;  // Invalid path segment
      }
    }
  }
  _currentNode = node;
  return true;
}

bool CommandMenuTree::executeCommand(const CommandRequest& request, std::string& response) {
  MenuNode* originalNode = _currentNode;
  navigate(request.getPath(), request.isAbsolute());

  Command* cmd = _currentNode->getCommand(request.getCommandName());
  if (cmd) {
    cmd->execute(request, response);
    _currentNode = originalNode;  // Reset to original position
    return true;
  }

  _currentNode = originalNode;
  return false;
}
