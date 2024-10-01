#pragma once

#include "CommandRequest.hpp"

class Command {
public:
  virtual ~Command() = default;
  virtual void execute(CommandRequest& request) = 0;
  virtual std::string getName() const = 0;
  virtual std::string getUsage() const = 0;
};
