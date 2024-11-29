#pragma once

#include <string>
#include "CommandIf.hpp"

namespace cliService {

  class User {
  public:
    User(std::string username, std::string password, AccessLevel accessLevel)
      : _username(std::move(username)), _password(std::move(password)), _accessLevel(accessLevel)
    {}

    const std::string& getUsername() const { return _username; }
    const std::string& getPassword() const { return _password; }
    AccessLevel getAccessLevel() const { return _accessLevel; }

  private:
    std::string _username;
    std::string _password;
    AccessLevel _accessLevel;
  };

}
