#pragma once

#include "Command.hpp"

class RgbLedCommand : public Command {
public:
  RgbLedCommand(Command::AccessLevel accessLevel) : Command(accessLevel) {}
  void execute(CommandRequest& request) override;
  std::string getName() const override;
  std::string getUsage() const override;
};
