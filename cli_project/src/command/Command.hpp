#pragma once

#include "../command/CommandRequest.hpp"
#include <string>

class Command {
public:
  enum class AccessLevel {
    Basic = 0,
    Advanced = 1,
    Admin = 2
  };

  Command(Command::AccessLevel accessLevel) : accessLevel(accessLevel) {}
  virtual ~Command() = default;

  virtual void execute(CommandRequest& request) = 0;
  virtual std::string getName() const = 0;
  virtual std::string getUsage() const = 0;
  
  Command::AccessLevel getAccessLevel() const { return accessLevel; }

private:
  Command::AccessLevel accessLevel;
};
