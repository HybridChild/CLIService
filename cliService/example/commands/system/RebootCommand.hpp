#pragma once
#include "cliService/tree/CommandIf.hpp"
#include "commands/AccessLevel.hpp"

namespace cliService
{

  class RebootCommand : public CommandIf
  {
  public:
    RebootCommand(std::string name, AccessLevel level, std::string description = "Reboot the device")
      : CommandIf(std::move(name), level, std::move(description))
    {}

    CommandResponse execute(const std::vector<std::string>& args) override
    {
      if (args.size() > 0) {
        return CommandResponse("Command take no arguments.", CommandStatus::InvalidArguments);
      }

      // signal system reboot

      return CommandResponse::success("System reboot initiated...");
    }
  };

}
