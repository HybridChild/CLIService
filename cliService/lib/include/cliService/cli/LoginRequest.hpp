#pragma once
#include "cliService/cli/RequestBase.hpp"
#include <string>
#include <cassert>

namespace cliService
{

  class LoginRequest : public RequestBase
  {
  public:
    explicit LoginRequest(const std::string& input)
    {
      size_t delimPos = input.find(':');
      assert(delimPos != std::string::npos && "Invalid login format");

      _username = input.substr(0, delimPos);
      _password = input.substr(delimPos + 1);

      assert(!_username.empty() && "Username cannot be empty");
      assert(!_password.empty() && "Password cannot be empty");
    }

    const std::string& getUsername() const { return _username; }
    const std::string& getPassword() const { return _password; }

  private:
    std::string _username;
    std::string _password;
  };

}
