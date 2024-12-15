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
    // Factory method to create LoginRequest
    static std::optional<LoginRequest> create(const std::string& input)
    {
      size_t delimPos = input.find(':');
      
      if (delimPos == std::string::npos) {
        return std::nullopt;
      }

      std::string username = input.substr(0, delimPos);
      std::string password = input.substr(delimPos + 1);
      
      if (username.empty() || password.empty()) {
        return std::nullopt;
      }

      return LoginRequest(std::move(username), std::move(password));
    }

    const std::string& getUsername() const { return _username; }
    const std::string& getPassword() const { return _password; }

  private:
    // Private constructor - use factory method instead
    LoginRequest(std::string username, std::string password)
      : _username(std::move(username))
      , _password(std::move(password))
    {}

    std::string _username;
    std::string _password;
  };

}
