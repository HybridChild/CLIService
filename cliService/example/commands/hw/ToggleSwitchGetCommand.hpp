#pragma once
#include "cliService/tree/CommandIf.hpp"
#include "commands/AccessLevel.hpp"
#include "util/util.hpp"

namespace cliService
{

  enum class ToggleSwitchPosition
  {
    Up,
    Center,
    Down,
  };

  class ToggleSwitchGetCommand : public CommandIf
  {
  public:
    ToggleSwitchGetCommand(std::string name, AccessLevel level, std::string description)
      : CommandIf(std::move(name), level, "Get toggle switch position - Args: <toggleSwitch ID>")
    {}

    CommandResponse execute(const std::vector<std::string>& args) override
    {
      if (args.size() != 1) {
        return CommandIf::createInvalidArgumentCountResponse(1);
      }

      if (!util::isIntegerString(args[0])) {
        return CommandResponse("\r\n\tInvalid toggle switch ID: " + args[0] + ". Must be integer.\r\n", CommandStatus::InvalidArguments);
      }

      uint32_t potId = std::stoi(args[0]);

      if (potId < 1 || potId > 2) {
        return CommandResponse("\r\n\tInvalid toggle switch ID: " + args[0] + ". Must be 1 .. 2\r\n", CommandStatus::InvalidArguments);
      }

      ToggleSwitchPosition togglePos = readToggleSwitchPosition(potId);
      return CommandResponse::success("\r\n\tToggle switch " + args[0] + " is in position: " + posToString(togglePos) + "\r\n");
    }

  private:
    std::string posToString(ToggleSwitchPosition pos) const
    {
      switch (pos)
      {
        case ToggleSwitchPosition::Up:
          return "Up";
        case ToggleSwitchPosition::Center:
          return "Center";
        case ToggleSwitchPosition::Down:
          return "Down";
        default:
          return "Unknown";
      }
    }

    ToggleSwitchPosition readToggleSwitchPosition(uint32_t id) const
    {
      ToggleSwitchPosition pos{};

      // read toggle switch position from hardware
      switch (id)
      {
        case 1:
          pos = ToggleSwitchPosition::Up;
          break;
        case 2:
          pos = ToggleSwitchPosition::Center;
          break;
        default:
          break;
      }

      return pos;
    }
  };

}
