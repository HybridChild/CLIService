#include "CLIService.hpp"
#include <iostream>

CLIService::CLIService(std::unique_ptr<CLIServiceConfiguration> conf) 
  : config(std::move(conf))
{
  tree = config->getMenuTree();
  io = config->getIOStream();
}

void CLIService::activate() {
  running = true;
  printWelcomeMessage();
}

void CLIService::service() {
  if (!isAuthenticated) {
    if (!authenticateUser()) {
      return;
    }
  }

  io->write(currentUser->getUsername() + "@" + tree->getCurrentPath() + " > ");
  std::string input = io->read();

  if (input == "exit") {
    printGoodbyeMessage();
    running = false;
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
      io->writeLine("Unknown command type.");
      break;
  }
}

void CLIService::handleNavigation(const CommandRequest& request) {
  if (request.getType() == CommandRequest::Type::RootNavigation) {
    tree->setCurrentNode(tree->getRoot());
    io->writeLine("Navigated to root: /");
  } else if (navigateToNode(request.getPath(), request.isAbsolute())) {
    io->writeLine("Current location: " + tree->getCurrentPath());
  }
}

void CLIService::handleExecution(const CommandRequest& request) {
  auto* tree = config->getMenuTree();
  MenuNode* originalNode = tree->getCurrentNode();

  if (navigateToNode(request.getPath(), request.isAbsolute())) {
    executeCommand(request.getCommandName(), request);
  }

  tree->setCurrentNode(originalNode);
}

bool CLIService::navigateToNode(const std::vector<std::string>& path, bool isAbsolute) {
  if (isAbsolute) {
    tree->setCurrentNode(tree->getRoot());
  }

  for (const auto& pathSegment : path) {
    if (pathSegment == "..") {
      MenuNode* parent = tree->getCurrentNode()->getParent();
      if (parent) {
        tree->setCurrentNode(parent);
      } else {
        io->writeLine("Already at root.");
        return false;
      }
    } else {
      MenuNode* nextNode = tree->getCurrentNode()->getSubMenu(pathSegment);
      if (nextNode && static_cast<int>(currentUser->getAccessLevel()) >= static_cast<int>(nextNode->getAccessLevel())) {
        tree->setCurrentNode(nextNode);
      } else {
        io->writeLine("Access denied or invalid path: " + pathSegment);
        return false;
      }
    }
  }
  return true;
}

void CLIService::executeCommand(const std::string& commandName, const CommandRequest& request) {
  Command* cmd = tree->getCurrentNode()->getCommand(commandName);
  
  if (cmd && static_cast<int>(currentUser->getAccessLevel()) >= static_cast<int>(cmd->getAccessLevel())) {
    CommandRequest processedRequest = tree->processRequest(request);
    printResponse(processedRequest);
  } else if (cmd) {
    io->writeLine("Access denied. Your access level is too low for this command.");
  } else {
    io->writeLine("Unknown command. Use 'help' for available commands.");
  }
}

void CLIService::listCurrentCommands() {
  io->writeLine("Current location: " + tree->getCurrentPath());
  io->writeLine("Available commands:");
  for (const auto& [name, cmd] : tree->getCurrentNode()->getCommands()) {
    if (static_cast<int>(currentUser->getAccessLevel()) >= static_cast<int>(cmd->getAccessLevel())) {
      io->writeLine("  " + name + " - Usage: " + cmd->getUsage());
    }
  }
  io->writeLine("Available submenus:");
  for (const auto& [name, submenu] : tree->getCurrentNode()->getSubMenus()) {
    if (static_cast<int>(currentUser->getAccessLevel()) >= static_cast<int>(submenu->getAccessLevel())) {
      io->writeLine("  " + name + "/");
    }
  }
}

void CLIService::printResponse(const CommandRequest& request) {
  if (!request.getResponse().empty()) {
    io->writeLine(request.getResponse());
  }
}

void CLIService::printWelcomeMessage() {
  io->writeLine("Welcome to the CLI Service. Type 'exit' to quit.");
}

void CLIService::printGoodbyeMessage() {
  io->writeLine("Thank you for using the CLI Service. Goodbye!");
}

bool CLIService::authenticateUser() {
  auto* io = config->getIOStream();
  std::string username, password;

  io->write("Username: ");
  username = io->read();
  io->write("Password: ");
  password = io->read();

  currentUser = config->authenticateUser(username, password);
  if (currentUser) {
    isAuthenticated = true;
    io->writeLine("Login successful. Welcome, " + username + "!");
    return true;
  } else {
    io->writeLine("Login failed. Invalid username or password.");
    return false;
  }
}
