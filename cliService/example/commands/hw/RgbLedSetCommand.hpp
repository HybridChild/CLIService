#pragma once
#include "cliService/tree/CommandIf.hpp"
#include "commands/AccessLevel.hpp"
#include "util/util.hpp"
#include <array>

namespace cliService
{

  class RgbLedSetCommand : public CommandIf
  {
  public:
    RgbLedSetCommand(std::string name, AccessLevel level)
      : CommandIf(std::move(name), level)
    {}

    CommandResponse execute(const std::vector<std::string>& args) override
    {
      if (args.size() < 1 || args.size() > 4)
      {
        return CommandResponse("Invalid number of arguments. Please provide: <ID> <R> <G> <B>", CommandStatus::InvalidArguments);
      }

      if (!util::isIntegerString(args[0]))
      {
        return CommandResponse("Invalid RGB LED ID: " + args[0] + ". Must be integer.", CommandStatus::InvalidArguments);
      }

      uint32_t rgbLedId = std::stoi(args[0]);

      if (rgbLedId < 1 || rgbLedId > 2)
      {
        return CommandResponse("Invalid ID: " + args[0] + " ... valid IDs: 1 .. 2", CommandStatus::InvalidArguments);
      }

      std::array<uint8_t, 3> rgbValues;

      for (size_t i = 0; i < 3; i++)
      {
        if (!isValidValueString(args[i + 1]))
        {
          return CommandResponse("Invalid value: " + args[i + 1] + " ... valid values: 0 .. 255", CommandStatus::InvalidArguments);
        }

        rgbValues[i] = std::stoi(args[i + 1]);
      }

      setRgbLed(rgbLedId, rgbValues);

      return CommandResponse::success("RGB LED " + args[0] + " set to: " + args[1] + " " + args[2] + " " + args[3]);
    }

  private:
    bool isValidValueString(const std::string& str)
    {
      if (str.empty() || str.length() > 3) return false;
      
      // Check if all characters are digits
      for (char c : str) {
        if (!std::isdigit(c)) return false;
      }
      
      // Convert to integer and check bounds
      int value = 0;
      for (char c : str)
      {
        value = value * 10 + (c - '0');
        if (value > 255) return false;
      }
      
      return true;
    }

    void setRgbLed(uint32_t id, std::array<uint8_t, 3> rgbValues) const
    {
      (void)id;
      (void)rgbValues;
      
      // Create RGB LED message and send to output service
    }
  };

}
