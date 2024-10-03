#include "PotmeterCommand.hpp"

void PotmeterCommand::execute(CommandRequest& request) {
  if (!request.getArgs().empty()) {
    request.setResponse("Error: potmeter command takes no arguments. Usage: " + getUsage() + "\n", 1);
    return;
  }
  // Simulate reading potmeter value
  request.setResponse("Potmeter value: 512\n", 0);
}

std::string PotmeterCommand::getName() const { return "potmeter"; }
std::string PotmeterCommand::getUsage() const { return "potmeter"; }
