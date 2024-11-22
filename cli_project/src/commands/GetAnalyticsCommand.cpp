#include "GetAnalyticsCommand.hpp"

void GetAnalyticsCommand::execute(const CommandRequest& request, std::string& response) {
  if (!request.getArgs().empty()) {
    response = "Error: getAnalytics command takes no arguments. Usage: " + getUsage() + "\n";
    return;
  }
  // Simulate reading analytics data
  response = "DataPoint[1] = User is an idiot\n";
}

std::string GetAnalyticsCommand::getName() const { return "getData"; }
std::string GetAnalyticsCommand::getUsage() const { return "getData"; }
