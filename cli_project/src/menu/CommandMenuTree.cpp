#include "CommandMenuTree.hpp"

CommandMenuTree::CommandMenuTree() : root("root"), currentNode(&root) {}

MenuNode* CommandMenuTree::getCurrentNode() { return currentNode; }
MenuNode* CommandMenuTree::getRoot() { return &root; }

void CommandMenuTree::processRequest(const CommandRequest& request) {
  switch (request.getType()) {
    case CommandRequest::Type::RootNavigation:
      currentNode = &root;
      std::cout << "Navigated to root: " << getCurrentPath() << std::endl;
      break;
    case CommandRequest::Type::Navigation:
      navigate(request.getPath(), request.isAbsolute());
      std::cout << "Navigated to: " << getCurrentPath() << std::endl;
      break;
    case CommandRequest::Type::Execution:
      if (executeCommand(const_cast<CommandRequest&>(request))) {
        std::cout << "Response: " << request.getResponse() 
                  << " (Code: " << request.getResponseCode() << ")" << std::endl;
      } else {
        std::cout << "Unknown command. Use 'help' for available commands." << std::endl;
      }
      break;
  }
}

std::string CommandMenuTree::getCurrentPath() {
  std::vector<std::string> path;
  MenuNode* node = currentNode;
  while (node != &root) {
    path.push_back(node->getName());
    node = node->getParent();
  }
  std::string result = "/";
  for (auto it = path.rbegin(); it != path.rend(); ++it) {
    result += *it + "/";
  }
  return result;
}

void CommandMenuTree::navigate(const std::vector<std::string>& path, bool isAbsolute) {
  MenuNode* node = isAbsolute ? &root : currentNode;

  for (const auto& segment : path) {
    if (segment == "..") {
      if (node != &root && node->getParent() != nullptr) {
        node = node->getParent();
      }
    } else if (!segment.empty()) {
      MenuNode* next = node->getSubMenu(segment);
      if (next) {
        node = next;
      } else {
        std::cout << "Invalid path segment: " << segment << std::endl;
        return;
      }
    }
  }
  currentNode = node;
}

bool CommandMenuTree::executeCommand(CommandRequest& request) {
  MenuNode* originalNode = currentNode;
  navigate(request.getPath(), request.isAbsolute());

  Command* cmd = currentNode->getCommand(request.getCommandName());
  if (cmd) {
    cmd->execute(request);
    currentNode = originalNode;  // Reset to original position
    return true;
  }

  currentNode = originalNode;
  return false;
}
