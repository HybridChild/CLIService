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
    ToggleSwitchGetCommand(std::string name, AccessLevel level, std::string description = "Get toggle switch position - Args: <toggleSwitch ID>")
      : CommandIf(std::move(name), level, std::move(description))
    {}

    CommandResponse execute(const std::vector<std::string>& args) override
    {
      if (args.size() < 1 || args.size() > 1) {
        return CommandResponse("Invalid number of arguments.", CommandStatus::InvalidArguments);
      }

      if (!util::isIntegerString(args[0])) {
        return CommandResponse("Invalid toggle switch ID: " + args[0] + ". Must be integer.", CommandStatus::InvalidArguments);
      }

      uint32_t potId = std::stoi(args[0]);

      if (potId < 1 || potId > 2) {
        return CommandResponse("Invalid toggle switch ID: " + args[0] + ". Must be 1 .. 2", CommandStatus::InvalidArguments);
      }

      ToggleSwitchPosition togglePos = readToggleSwitchPosition(potId);
      return CommandResponse::success("Toggle switch " + args[0] + " is in position: " + posToString(togglePos));
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
