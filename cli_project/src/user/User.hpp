#pragma once

#include "../command/Command.hpp"
#include <string>

class User {
public:
  User(std::string username, std::string password, Command::AccessLevel accessLevel)
    : username(std::move(username)), password(std::move(password)), accessLevel(accessLevel) {}

  const std::string& getUsername() const { return username; }
  const std::string& getPassword() const { return password; }
  Command::AccessLevel getAccessLevel() const { return accessLevel; }

private:
  std::string username;
  std::string password;
  Command::AccessLevel accessLevel;
};
