#pragma once

#include <string>
#include "CommandRequest.hpp"

namespace cliService {

  enum class AccessLevel;

  class Command {
  public:
    Command(AccessLevel accessLevel) : _accessLevel(accessLevel) {}
    virtual ~Command() = default;

    virtual void execute(const CommandRequest& request, std::string& response) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getUsage() const = 0;
    
    AccessLevel getAccessLevel() const { return _accessLevel; }

  private:
    AccessLevel _accessLevel;
  };

}
