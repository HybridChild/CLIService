#pragma once

#include "CommandRequest.hpp"
#include <string>

class Command {
public:
  enum class AccessLevel {
    Basic = 0,
    Advanced = 1,
    Admin = 2
  };

  Command(Command::AccessLevel accessLevel) : _accessLevel(accessLevel) {}
  virtual ~Command() = default;

  virtual void execute(const CommandRequest& request, std::string& response) = 0;
  virtual std::string getName() const = 0;
  virtual std::string getUsage() const = 0;
  
  Command::AccessLevel getAccessLevel() const { return _accessLevel; }

private:
  Command::AccessLevel _accessLevel;
};
