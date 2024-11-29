#pragma once

#include "cliService/CommandIf.hpp"
#include "commands/AccessLevel.hpp"

namespace cliService {

  class GetAnalyticsCommand : public Command {
  public:
    GetAnalyticsCommand(AccessLevel accessLevel) : Command(accessLevel) {}
    void execute(const CommandRequest& request, std::string& response) override;
    std::string getName() const override;
    std::string getUsage() const override;
  };

}
