#include "CLIService.hpp"
#include <iostream>

CLIService::CLIService(std::unique_ptr<CLIServiceConfiguration> config) 
  : configuration(std::move(config))
{}

void CLIService::activate() {
  running = true;
  configuration->getIOStream()->write("Welcome to the CLI Service. Type 'exit' to quit.\n");
}

void CLIService::service() {
  configuration->getIOStream()->write(configuration->getMenuTree()->getCurrentPath() + " > ");
  std::string input = configuration->getIOStream()->read();

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
  CommandRequest processedRequest = configuration->getMenuTree()->processRequest(request);
  printResponse(processedRequest);
}

void CLIService::listCurrentCommands() {
  configuration->getIOStream()->write("Current location: " + configuration->getMenuTree()->getCurrentPath() + "\n");
  configuration->getIOStream()->write("Available commands:\n");

  for (const auto& [name, cmd] : configuration->getMenuTree()->getCurrentNode()->getCommands()) {
    configuration->getIOStream()->write("  " + name + " - Usage: " + cmd->getUsage() + "\n");
  }

  configuration->getIOStream()->write("Available submenus:\n");
  for (const auto& [name, submenu] : configuration->getMenuTree()->getCurrentNode()->getSubMenus()) {
    configuration->getIOStream()->write("  " + name + "/\n");
  }
}

void CLIService::printResponse(const CommandRequest& request) {
  if (!request.getResponse().empty()) {
    configuration->getIOStream()->write(request.getResponse() + "\n");
  }
}
