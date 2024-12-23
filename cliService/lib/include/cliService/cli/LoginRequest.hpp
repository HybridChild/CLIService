#pragma once
#include "cliService/cli/RequestBase.hpp"
#include <string>
#include <optional>

namespace cliService
{

  // Simple request type to indicate invalid login attempt
  class InvalidLoginRequest : public RequestBase
  {
  public:
    InvalidLoginRequest() = default;
  };

  class LoginRequest : public RequestBase
  {
  public:
    LoginRequest(std::string username, std::string password)
      : _username(std::move(username))
      , _password(std::move(password))
    {}

    const std::string& getUsername() const { return _username; }
    const std::string& getPassword() const { return _password; }

  private:
    std::string _username;
    std::string _password;
  };

}
