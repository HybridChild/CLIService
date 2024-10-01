#pragma once

#include "Command.hpp"

class PotmeterCommand : public Command {
public:
  void execute(CommandRequest& request) override;
  std::string getName() const override;
  std::string getUsage() const override;
};
