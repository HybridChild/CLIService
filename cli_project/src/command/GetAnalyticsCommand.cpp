#include "GetAnalyticsCommand.hpp"

void GetAnalyticsCommand::execute(CommandRequest& request) {
  if (!request.getArgs().empty()) {
    request.setResponse("Error: getAnalytics command takes no arguments. Usage: " + getUsage() + "\n", 1);
    return;
  }
  // Simulate reading analytics data
  request.setResponse("DataPoint[1] = User is an idiot\n", 0);
}

std::string GetAnalyticsCommand::getName() const { return "getData"; }
std::string GetAnalyticsCommand::getUsage() const { return "getData"; }
