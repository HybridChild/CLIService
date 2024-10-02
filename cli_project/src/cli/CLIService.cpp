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

  io->write(currentUser + "@" + tree->getCurrentPath() + " > ");
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
  CommandRequest processedRequest = tree->processRequest(request);
  printResponse(processedRequest);
}

void CLIService::listCurrentCommands() {
  io->write("Current location: " + tree->getCurrentPath() + "\n");
  io->write("Available commands:\n");

  for (const auto& [name, cmd] : tree->getCurrentNode()->getCommands()) {
    io->write("  " + name + " - Usage: " + cmd->getUsage() + "\n");
  }

  io->write("Available submenus:\n");
  for (const auto& [name, submenu] : tree->getCurrentNode()->getSubMenus()) {
    io->write("  " + name + "/\n");
  }
}

void CLIService::printResponse(const CommandRequest& request) {
  if (!request.getResponse().empty()) {
    io->write(request.getResponse() + "\n");
  }
}

void CLIService::printWelcomeMessage() {
  io->write("Welcome to the CLI Service. Type 'exit' to quit.\n");
}

void CLIService::printGoodbyeMessage() {
  io->write("Thank you for using the CLI Service. Goodbye!\n");
}

bool CLIService::authenticateUser() {
  auto* io = config->getIOStream();
  std::string username, password;

  io->write("Username: ");
  username = io->read();
  io->write("Password: ");
  password = io->read();

  if (config->authenticateUser(username, password)) {
    isAuthenticated = true;
    currentUser = username;
    io->write("Login successful. Welcome, " + username + "!\n");
    return true;
  } else {
    io->write("Login failed. Invalid username or password.\n");
    return false;
  }
}
