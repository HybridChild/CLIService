#pragma once

#include "cliService/requests/RequestBase.hpp"
#include <string>

namespace cliService
{

  class LoginRequest : public RequestBase
  {
  public:
    explicit LoginRequest(const std::string& input);

    bool isExitRequest() const { return _isExitRequest; }
    const std::string& getUsername() const { return _username; }
    const std::string& getPassword() const { return _password; }

  private:
    std::string _username;
    std::string _password;
    bool _isExitRequest;
  };

}
