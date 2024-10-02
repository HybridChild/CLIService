#pragma once

#include "Command.hpp"

class PotmeterCommand : public Command {
public:
  PotmeterCommand(Command::AccessLevel accessLevel) : Command(accessLevel) {}
  void execute(CommandRequest& request) override;
  std::string getName() const override;
  std::string getUsage() const override;
};
