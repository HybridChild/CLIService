#include "cliService/user/User.hpp"

namespace cliService
{

  User::User(std::string username, std::string password, AccessLevel level)
    : _username(std::move(username))
    , _password(std::move(password))
    , _accessLevel(level)
  {}

  const std::string& User::getUsername() const { return _username; }

  const std::string& User::getPassword() const { return _password; }
  
  AccessLevel User::getAccessLevel() const { return _accessLevel; }

}
