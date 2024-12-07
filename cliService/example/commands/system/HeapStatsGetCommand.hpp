#pragma once
#include "cliService/tree/CommandIf.hpp"
#include "commands/AccessLevel.hpp"

namespace cliService
{

  class HeapStatsGetCommand : public CommandIf
  {
  public:
    HeapStatsGetCommand(std::string name, AccessLevel level)
      : CommandIf(std::move(name), level)
    {}

    CommandResponse execute(const std::vector<std::string>& args) override
    {
      if (args.size() > 0)
      {
        return CommandResponse("Command does not take any arguments.", CommandStatus::InvalidArguments);
      }

      std::string response = "Heap stats: \n";
      response += "Available Heap Space In Bytes: \n";
      response += "Size Of Largest Free Block In Bytes: \n";
      response += "Size Of Smallest Free Block In Bytes: \n";
      response += "Number Of Free Blocks: \n";
      response += "Minimum Ever Free Bytes Remaining: \n";
      response += "Number Of Successful Allocations: \n";
      response += "Number Of Successful Frees: ";
      return CommandResponse::success(response);
    }
  };

}
