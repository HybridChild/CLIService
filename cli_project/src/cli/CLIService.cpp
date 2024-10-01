#include "CLIService.hpp"
#include "../command/CommandRequest.hpp"

CLIService::CLIService(std::unique_ptr<CommandMenuTree> tree) 
  : menuTree(std::move(tree)) {}

void CLIService::processCommand(const std::string& input) {
  CommandRequest request(input);
  menuTree->processRequest(request);
}

void CLIService::listCurrentCommands() {
  std::cout << "Current location: " << menuTree->getCurrentPath() << std::endl;
  std::cout << "Available commands:" << std::endl;
  for (const auto& [name, cmd] : menuTree->getCurrentNode()->getCommands()) {
    std::cout << "  " << name << " - Usage: " << cmd->getUsage() << std::endl;
  }
  std::cout << "Available submenus:" << std::endl;
  for (const auto& [name, submenu] : menuTree->getCurrentNode()->getSubMenus()) {
    std::cout << "  " << name << "/" << std::endl;
  }
}

void CLIService::run() {
  std::cout << "Welcome to the CLI Service. Type 'exit' to quit." << std::endl;
  std::string input;
  while (true) {
    std::cout << menuTree->getCurrentPath() << " > ";
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
