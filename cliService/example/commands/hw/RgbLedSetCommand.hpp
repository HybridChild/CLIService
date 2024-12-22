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
    RgbLedSetCommand(std::string name, AccessLevel level, std::string description = "")
      : CommandIf(std::move(name), level, "Set RGB LED color - Args: <rgbLED ID> <R> <G> <B>")
    {}

    Response execute(const std::vector<std::string>& args) override
    {
      if (args.size() != 4) {
        return CommandIf::createInvalidArgumentCountResponse(4);
      }

      if (!util::isIntegerString(args[0])) {
        return Response("\r\n\tInvalid RGB LED ID: " + args[0] + ". Must be integer.\r\n", ResponseStatus::InvalidArguments);
      }

      uint32_t rgbLedId = std::stoi(args[0]);

      if (rgbLedId < 1 || rgbLedId > 2) {
        return Response("\r\n\tInvalid ID: " + args[0] + " ... valid IDs: 1 .. 2\r\n", ResponseStatus::InvalidArguments);
      }

      std::array<uint8_t, 3> rgbValues;

      for (size_t i = 0; i < 3; i++)
      {
        if (!isValidValueString(args[i + 1])) {
          return Response("\r\n\tInvalid value: " + args[i + 1] + " ... valid values: 0 .. 255\r\n", ResponseStatus::InvalidArguments);
        }

        rgbValues[i] = std::stoi(args[i + 1]);
      }

      setRgbLed(rgbLedId, rgbValues);

      return Response::success("\r\n\tRGB LED " + args[0] + " set to: " + args[1] + " " + args[2] + " " + args[3] + "\r\n");
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
      uint32_t value = 0;
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
