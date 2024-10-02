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

  // Handle navigation commands
  if (request.getType() == CommandRequest::Type::Navigation) {
    if (request.isAbsolute()) {
      tree->setCurrentNode(tree->getRoot());
    }
    for (const auto& pathSegment : request.getPath()) {
      if (pathSegment == "..") {
        MenuNode* parent = tree->getCurrentNode()->getParent();
        if (parent) {
          tree->setCurrentNode(parent);
        } else {
          io->writeLine("Already at root.");
        }
      } else {
        MenuNode* nextNode = tree->getCurrentNode()->getSubMenu(pathSegment);
        if (nextNode) {
          tree->setCurrentNode(nextNode);
        } else {
          io->writeLine("Invalid path: " + pathSegment);
          return;
        }
      }
    }
    io->writeLine("Current location: " + tree->getCurrentPath());
    return;
  }

  // Handle execution commands
  MenuNode* originalNode = tree->getCurrentNode();
  
  // Navigate to the correct node based on the request path
  for (const auto& pathSegment : request.getPath()) {
    MenuNode* nextNode = tree->getCurrentNode()->getSubMenu(pathSegment);
    if (!nextNode) {
      io->writeLine("Invalid path: " + pathSegment);
      tree->setCurrentNode(originalNode);  // Reset to original position
      return;
    }
    tree->setCurrentNode(nextNode);
  }
  
  // Now we're at the correct node, look for the command
  Command* cmd = tree->getCurrentNode()->getCommand(request.getCommandName());
  
  if (cmd && currentUser && static_cast<int>(currentUser->getAccessLevel()) >= static_cast<int>(cmd->getAccessLevel())) {
    CommandRequest processedRequest = tree->processRequest(request);
    printResponse(processedRequest);
  } else if (cmd) {
    io->writeLine("Access denied. Your access level is too low for this command.");
  } else {
    io->writeLine("Unknown command. Use 'help' for available commands.");
  }
  
  // Reset to original position
  tree->setCurrentNode(originalNode);
}

void CLIService::listCurrentCommands() {
  io->writeLine("Current location: " + tree->getCurrentPath());
  io->writeLine("Available commands:");
  for (const auto& [name, cmd] : tree->getCurrentNode()->getCommands()) {
    if (currentUser && static_cast<int>(currentUser->getAccessLevel()) >= static_cast<int>(cmd->getAccessLevel())) {
      io->writeLine("  " + name + " - Usage: " + cmd->getUsage());
    }
  }
  io->writeLine("Available submenus:");
  for (const auto& [name, submenu] : tree->getCurrentNode()->getSubMenus()) {
    io->writeLine("  " + name + "/");
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
