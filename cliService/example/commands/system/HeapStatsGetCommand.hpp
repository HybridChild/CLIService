#pragma once
#include "cliService/tree/CommandIf.hpp"
#include "commands/AccessLevel.hpp"

namespace cliService
{

  class HeapStatsGetCommand : public CommandIf
  {
  public:
    HeapStatsGetCommand(std::string name, AccessLevel level, std::string description = "")
      : CommandIf(std::move(name), level, "List FreeRTOS heap statistics")
    {}

    CommandResponse execute(const std::vector<std::string>& args) override
    {
      if (args.size() != 0) {
        return CommandIf::createInvalidArgumentCountResponse(0);
      }

      std::string response = "\r\n";
      response += "\tAvailable Heap Space In Bytes        : " + std::to_string(18500) + "\r\n";
      response += "\tSize Of Largest Free Block In Bytes  : " + std::to_string(12800) + "\r\n";
      response += "\tSize Of Smallest Free Block In Bytes : " + std::to_string(500) + "\r\n";
      response += "\tNumber Of Free Blocks                : " + std::to_string(4) + "\r\n";
      response += "\tMinimum Ever Free Bytes Remaining    : " + std::to_string(7700) + "\r\n";
      response += "\tNumber Of Successful Allocations     : " + std::to_string(123456) + "\r\n";
      response += "\tNumber Of Successful Frees           : " + std::to_string(23456) + "\r\n";
      return CommandResponse::success(response);
    }
  };

}
