#include "CLIService.hpp"
#include <iostream>

CLIService::CLIService(std::unique_ptr<CLIServiceConfiguration> config) 
  : configuration(std::move(config)) {}

void CLIService::processCommand(const std::string& input) {
  CommandRequest request(input);
  CommandRequest processedRequest = configuration->menuTree->processRequest(request);
  printResponse(processedRequest);
}

void CLIService::listCurrentCommands() {
  std::cout << "Current location: " << configuration->menuTree->getCurrentPath() << std::endl;
  std::cout << "Available commands:" << std::endl;
  for (const auto& [name, cmd] : configuration->menuTree->getCurrentNode()->getCommands()) {
    std::cout << "  " << name << " - Usage: " << cmd->getUsage() << std::endl;
  }
  std::cout << "Available submenus:" << std::endl;
  for (const auto& [name, submenu] : configuration->menuTree->getCurrentNode()->getSubMenus()) {
    std::cout << "  " << name << "/" << std::endl;
  }
}

void CLIService::run() {
  std::cout << "Welcome to the CLI Service. Type 'exit' to quit." << std::endl;
  std::string input;
  while (true) {
    std::cout << configuration->menuTree->getCurrentPath() << " > ";
    std::getline(std::cin, input);
    if (input == "exit") {
      break;
    } else if (input == "help") {
      listCurrentCommands();
    } else {
      processCommand(input);
    }
  }
  std::cout << "Thank you for using the CLI Service. Goodbye!" << std::endl;
}

void CLIService::printResponse(const CommandRequest& request) {
  if (!request.getResponse().empty()) {
    std::cout << "Response: " << request.getResponse() 
              << " (Code: " << request.getResponseCode() << ")" << std::endl;
  }
}