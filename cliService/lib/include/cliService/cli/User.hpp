#pragma once
#include <string>

namespace cliService
{

  enum class AccessLevel;  // Forward declaration - to be defined by library user

  class User
  {
  public:
    User(std::string username, std::string password, AccessLevel level)
      : _username(std::move(username))
      , _password(std::move(password))
      , _accessLevel(level)
    {}

    const std::string& getUsername() const { return _username; };
    const std::string& getPassword() const { return _password; };
    AccessLevel getAccessLevel() const { return _accessLevel; };

  private:
    std::string _username;
    std::string _password;
    AccessLevel _accessLevel;
  };

}
