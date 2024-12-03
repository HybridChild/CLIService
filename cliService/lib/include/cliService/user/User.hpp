#pragma once
#include <string>


namespace cliService
{

  // Forward declaration - to be defined by library user
  enum class AccessLevel;

  class User
  {
  public:
    User(std::string username, std::string password, AccessLevel level);

    const std::string& getUsername() const;
    const std::string& getPassword() const;
    AccessLevel getAccessLevel() const;

  private:
    std::string _username;
    std::string _password;
    AccessLevel _accessLevel;
  };

}
