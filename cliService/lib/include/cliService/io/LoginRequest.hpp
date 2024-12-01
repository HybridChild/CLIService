#pragma once

#include "cliService/io/RequestBase.hpp"
#include <string>

namespace cliService
{

  class LoginRequest : public RequestBase
  {
  public:
    explicit LoginRequest(const std::string& input);

    bool isExitRequest() const;
    const std::string& getUsername() const;
    const std::string& getPassword() const;

  private:
    std::string _username;
    std::string _password;
    bool _isExitRequest;
  };

}
