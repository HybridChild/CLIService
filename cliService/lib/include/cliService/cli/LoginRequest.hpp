#pragma once
#include "cliService/cli/RequestBase.hpp"
#include <string>

namespace cliService
{

  class LoginRequest : public RequestBase
  {
  public:
    explicit LoginRequest(const std::string& input);

    const std::string& getUsername() const { return _username; }
    const std::string& getPassword() const { return _password; }

  private:
    std::string _username;
    std::string _password;
  };

}
