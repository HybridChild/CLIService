#include "cliService/requests/LoginRequest.hpp"
#include <cassert>

namespace cliService
{

  LoginRequest::LoginRequest(const std::string& input)
    : _isExitRequest(false)
  {
    if (input == "exit")
    {
      _isExitRequest = true;
      return;
    }

    size_t delimPos = input.find(':');
    assert(delimPos != std::string::npos && "Invalid login format");

    _username = input.substr(0, delimPos);
    _password = input.substr(delimPos + 1);
    
    assert(!_username.empty() && "Username cannot be empty");
    assert(!_password.empty() && "Password cannot be empty");
  }

  bool LoginRequest::isExitRequest() const
  {
    return _isExitRequest;
  }

  const std::string& LoginRequest::getUsername() const
  {
    return _username;
  }

  const std::string& LoginRequest::getPassword() const
  {
    return _password;
  }

}
