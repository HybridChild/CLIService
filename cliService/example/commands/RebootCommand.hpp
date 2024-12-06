#pragma once
#include "cliService/tree/CommandIf.hpp"
#include "commands/AccessLevel.hpp"

namespace cliService
{

  class RebootCommand : public CommandIf
  {
  public:
    RebootCommand(std::string name, AccessLevel level)
      : CommandIf(std::move(name), level)
    {}

    CommandResponse execute(const std::vector<std::string>& args) override
    {
      if (args.size() > 0)
      {
        return CommandResponse("Reboot command does not take any arguments.", CommandStatus::InvalidArguments);
      }

      // signal system reboot
      return CommandResponse::success("System reboot initiated...");
    }
  };

}
