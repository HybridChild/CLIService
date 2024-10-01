#include "CLIService.hpp"
#include <iostream>

CLIService::CLIService(std::unique_ptr<CLIServiceConfiguration> config) 
  : configuration(std::move(config))
{}

void CLIService::activate() {
  running = true;
  configuration->inOutStream->write("Welcome to the CLI Service. Type 'exit' to quit.\n");
}

void CLIService::service() {
  configuration->inOutStream->write(configuration->menuTree->getCurrentPath() + " > ");
  std::string input = configuration->inOutStream->read();

  if (input == "exit") {
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
  CommandRequest processedRequest = configuration->menuTree->processRequest(request);
  printResponse(processedRequest);
}

void CLIService::listCurrentCommands() {
  configuration->inOutStream->write("Current location: " + configuration->menuTree->getCurrentPath() + "\n");
  configuration->inOutStream->write("Available commands:\n");

  for (const auto& [name, cmd] : configuration->menuTree->getCurrentNode()->getCommands()) {
    configuration->inOutStream->write("  " + name + " - Usage: " + cmd->getUsage() + "\n");
  }

  configuration->inOutStream->write("Available submenus:\n");
  for (const auto& [name, submenu] : configuration->menuTree->getCurrentNode()->getSubMenus()) {
    configuration->inOutStream->write("  " + name + "/\n");
  }
}

void CLIService::printResponse(const CommandRequest& request) {
  if (!request.getResponse().empty()) {
    configuration->inOutStream->write(request.getResponse() + "\n");
  }
}
