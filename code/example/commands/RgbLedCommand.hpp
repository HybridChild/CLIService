#pragma once

#include "cliService/CommandIf.hpp"
#include "commands/AccessLevel.hpp"

namespace cliService {

  class RgbLedCommand : public CommandIf {
  public:
    RgbLedCommand(const std::string& name, AccessLevel accessLevel)
      : CommandIf(name, accessLevel)
    {}

    void execute(const CommandRequest& request, std::string& response) override;
    std::string getUsage() const override;
  };

}
