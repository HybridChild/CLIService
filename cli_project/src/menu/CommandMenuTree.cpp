#include "CommandMenuTree.hpp"

CommandMenuTree::CommandMenuTree() : root("root"), currentNode(&root) {}

MenuNode* CommandMenuTree::getCurrentNode() { return currentNode; }
MenuNode* CommandMenuTree::getRoot() { return &root; }

CommandRequest CommandMenuTree::processRequest(const CommandRequest& request) {
  CommandRequest processedRequest = request;
  switch (request.getType()) {
    case CommandRequest::Type::RootNavigation:
      currentNode = &root;
      break;
    case CommandRequest::Type::Navigation:
      if (!navigate(request.getPath(), request.isAbsolute())) {
        processedRequest.setResponse("Navigation failed: Invalid path", 1);
      }
      break;
    case CommandRequest::Type::Execution:
      if (executeCommand(processedRequest)) {
        // Response is set by the command itself
      } else {
        processedRequest.setResponse("Unknown command. Use 'help' for available commands.", 1);
      }
      break;
  }
  return processedRequest;
}

std::string CommandMenuTree::getCurrentPath() const {
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

bool CommandMenuTree::navigate(const std::vector<std::string>& path, bool isAbsolute) {
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
        return false;  // Invalid path segment
      }
    }
  }
  currentNode = node;
  return true;
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
