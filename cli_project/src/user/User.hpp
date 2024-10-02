#pragma once

#include "../command/Command.hpp"
#include <string>

class User {
public:
  User(std::string username, std::string password, Command::AccessLevel accessLevel)
    : _username(std::move(username)), _password(std::move(password)), _accessLevel(accessLevel) {}

  const std::string& getUsername() const { return _username; }
  const std::string& getPassword() const { return _password; }
  Command::AccessLevel getAccessLevel() const { return _accessLevel; }

private:
  std::string _username;
  std::string _password;
  Command::AccessLevel _accessLevel;
};
