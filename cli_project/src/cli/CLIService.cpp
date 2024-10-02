#include "CLIService.hpp"
#include <iostream>

CLIService::CLIService(std::unique_ptr<CLIServiceConfiguration> conf) 
  : config(std::move(conf))
{}

void CLIService::activate() {
  running = true;
  config->getIOStream()->write("Welcome to the CLI Service. Type 'exit' to quit.\n");
}

void CLIService::service() {
  config->getIOStream()->write(config->getMenuTree()->getCurrentPath() + " > ");
  std::string input = config->getIOStream()->read();

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
  CommandRequest processedRequest = config->getMenuTree()->processRequest(request);
  printResponse(processedRequest);
}

void CLIService::listCurrentCommands() {
  config->getIOStream()->write("Current location: " + config->getMenuTree()->getCurrentPath() + "\n");
  config->getIOStream()->write("Available commands:\n");

  for (const auto& [name, cmd] : config->getMenuTree()->getCurrentNode()->getCommands()) {
    config->getIOStream()->write("  " + name + " - Usage: " + cmd->getUsage() + "\n");
  }

  config->getIOStream()->write("Available submenus:\n");
  for (const auto& [name, submenu] : config->getMenuTree()->getCurrentNode()->getSubMenus()) {
    config->getIOStream()->write("  " + name + "/\n");
  }
}

void CLIService::printResponse(const CommandRequest& request) {
  if (!request.getResponse().empty()) {
    config->getIOStream()->write(request.getResponse() + "\n");
  }
}
