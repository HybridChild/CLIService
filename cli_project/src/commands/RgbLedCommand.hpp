#pragma once

#include "../menu/Command.hpp"

class RgbLedCommand : public Command {
public:
  RgbLedCommand(Command::AccessLevel accessLevel) : Command(accessLevel) {}
  void execute(const CommandRequest& request, std::string& response) override;
  std::string getName() const override;
  std::string getUsage() const override;
};
