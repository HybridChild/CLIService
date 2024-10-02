#include "CLIService.hpp"
#include <iostream>

CLIService::CLIService(std::unique_ptr<CLIServiceConfiguration> config)
  : _config(std::move(config))
{
  _tree = config->getMenuTree();
  _io = config->getIOStream();
}

void CLIService::activate() {
  _running = true;
  printWelcomeMessage();
}

void CLIService::service() {
  if (!_isAuthenticated) {
    if (!authenticateUser()) {
      return;
    }
  }

  _io->write(_currentUser->getUsername() + "@" + _tree->getCurrentPath() + " > ");
  std::string input = _io->read();

  if (input == "exit") {
    printGoodbyeMessage();
    _running = false;
    return;
  } else if (input == "help") {
    listCurrentCommands();
  } else {
    processCommand(input);
  }
}

void CLIService::processCommand(const std::string& input) {
  CommandRequest request(input);
  
  switch (request.getType()) {
    case CommandRequest::Type::Navigation:
    case CommandRequest::Type::RootNavigation:
      handleNavigation(request);
      break;
    case CommandRequest::Type::Execution:
      handleExecution(request);
      break;
    default:
      _io->writeLine("Unknown command type.");
      break;
  }
}

void CLIService::handleNavigation(const CommandRequest& request) {
  if (request.getType() == CommandRequest::Type::RootNavigation) {
    _tree->setCurrentNode(_tree->getRoot());
    _io->writeLine("Navigated to root: /");
  } else if (navigateToNode(request.getPath(), request.isAbsolute())) {
    _io->writeLine("Current location: " + _tree->getCurrentPath());
  }
}

void CLIService::handleExecution(const CommandRequest& request) {
  auto* tree = _config->getMenuTree();
  MenuNode* originalNode = tree->getCurrentNode();

  if (navigateToNode(request.getPath(), request.isAbsolute())) {
    executeCommand(request.getCommandName(), request);
  }

  tree->setCurrentNode(originalNode);
}

bool CLIService::navigateToNode(const std::vector<std::string>& path, bool isAbsolute) {
  if (isAbsolute) {
    _tree->setCurrentNode(_tree->getRoot());
  }

  for (const auto& pathSegment : path) {
    if (pathSegment == "..") {
      MenuNode* parent = _tree->getCurrentNode()->getParent();
      if (parent) {
        _tree->setCurrentNode(parent);
      } else {
        _io->writeLine("Already at root.");
        return false;
      }
    } else {
      MenuNode* nextNode = _tree->getCurrentNode()->getSubMenu(pathSegment);
      if (nextNode && static_cast<int>(_currentUser->getAccessLevel()) >= static_cast<int>(nextNode->getAccessLevel())) {
        _tree->setCurrentNode(nextNode);
      } else {
        _io->writeLine("Access denied or invalid path: " + pathSegment);
        return false;
      }
    }
  }
  return true;
}

void CLIService::executeCommand(const std::string& commandName, const CommandRequest& request) {
  Command* cmd = _tree->getCurrentNode()->getCommand(commandName);
  
  if (cmd && static_cast<int>(_currentUser->getAccessLevel()) >= static_cast<int>(cmd->getAccessLevel())) {
    CommandRequest processedRequest = _tree->processRequest(request);
    printResponse(processedRequest);
  } else if (cmd) {
    _io->writeLine("Access denied. Your access level is too low for this command.");
  } else {
    _io->writeLine("Unknown command. Use 'help' for available commands.");
  }
}

void CLIService::listCurrentCommands() {
  _io->writeLine("Current location: " + _tree->getCurrentPath());
  _io->writeLine("Available commands:");
  for (const auto& [name, cmd] : _tree->getCurrentNode()->getCommands()) {
    if (static_cast<int>(_currentUser->getAccessLevel()) >= static_cast<int>(cmd->getAccessLevel())) {
      _io->writeLine("  " + name + " - Usage: " + cmd->getUsage());
    }
  }
  _io->writeLine("Available submenus:");
  for (const auto& [name, submenu] : _tree->getCurrentNode()->getSubMenus()) {
    if (static_cast<int>(_currentUser->getAccessLevel()) >= static_cast<int>(submenu->getAccessLevel())) {
      _io->writeLine("  " + name + "/");
    }
  }
}

void CLIService::printResponse(const CommandRequest& request) {
  if (!request.getResponse().empty()) {
    _io->writeLine(request.getResponse());
  }
}

void CLIService::printWelcomeMessage() {
  _io->writeLine("Welcome to the CLI Service. Type 'exit' to quit.");
}

void CLIService::printGoodbyeMessage() {
  _io->writeLine("Thank you for using the CLI Service. Goodbye!");
}

bool CLIService::authenticateUser() {
  std::string username, password;

  _io->write("Username: ");
  username = _io->read();
  _io->write("Password: ");
  password = _io->read();

  _currentUser = _config->authenticateUser(username, password);
  if (_currentUser) {
    _isAuthenticated = true;
    _io->writeLine("Login successful. Welcome, " + username + "!");
    return true;
  } else {
    _io->writeLine("Login failed. Invalid username or password.");
    return false;
  }
}
