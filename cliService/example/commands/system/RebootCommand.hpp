#pragma once
#include "cliService/tree/CommandIf.hpp"
#include "commands/AccessLevel.hpp"

namespace cliService
{

  class RebootCommand : public CommandIf
  {
  public:
    RebootCommand(std::string name, AccessLevel level, std::string description = "")
      : CommandIf(std::move(name), level, "Reboot the device")
    {
      (void)description;
    }

    CLIResponse execute(const std::vector<std::string>& args) override
    {
      if (args.size() != 0) {
        return CommandIf::createInvalidArgumentCountResponse(0);
      }

      // signal system reboot

      return CLIResponse::success(std::string("System reboot initiated..."));
    }
  };

}
