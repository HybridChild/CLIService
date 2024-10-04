#pragma once

#include "Command.hpp"

class GetAnalyticsCommand : public Command {
public:
  GetAnalyticsCommand(Command::AccessLevel accessLevel) : Command(accessLevel) {}
  void execute(const CommandRequest& request, std::string& response) override;
  std::string getName() const override;
  std::string getUsage() const override;
};
