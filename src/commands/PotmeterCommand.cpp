#include "PotmeterCommand.hpp"

namespace cliService {
  
  void PotmeterCommand::execute(const CommandRequest& request, std::string& response) {
    if (!request.getArgs().empty()) {
      response = "Error: potmeter command takes no arguments. Usage: " + getUsage() + "\n";
      return;
    }
    // Simulate reading potmeter value
    response = "Potmeter value: 512\n";
  }

  std::string PotmeterCommand::getName() const { return "potmeter"; }
  std::string PotmeterCommand::getUsage() const { return "potmeter"; }

}
