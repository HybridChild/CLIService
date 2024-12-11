#pragma once
#include "cliService/tree/CommandIf.hpp"
#include "commands/AccessLevel.hpp"

namespace cliService
{

  class HeapStatsGetCommand : public CommandIf
  {
  public:
    HeapStatsGetCommand(std::string name, AccessLevel level, std::string description = "")
      : CommandIf(std::move(name), level, "Get heap statistics")
    {}

    CommandResponse execute(const std::vector<std::string>& args) override
    {
      if (args.size() > 0)
      {
        return CommandResponse("Command takes no arguments.", CommandStatus::InvalidArguments);
      }

      std::string response = "Heap stats: \n";
      response += "  Available Heap Space In Bytes:        18500\n";
      response += "  Size Of Largest Free Block In Bytes:  12800\n";
      response += "  Size Of Smallest Free Block In Bytes:   500\n";
      response += "  Number Of Free Blocks:                    4\n";
      response += "  Minimum Ever Free Bytes Remaining:     7700\n";
      response += "  Number Of Successful Allocations:    123456\n";
      response += "  Number Of Successful Frees:           23456";
      return CommandResponse::success(response);
    }
  };

}
