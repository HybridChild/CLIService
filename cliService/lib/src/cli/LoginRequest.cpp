#include "cliService/cli/LoginRequest.hpp"
#include <cassert>

namespace cliService
{

  LoginRequest::LoginRequest(const std::string& input)
  {
    size_t delimPos = input.find(':');
    assert(delimPos != std::string::npos && "Invalid login format");

    _username = input.substr(0, delimPos);
    _password = input.substr(delimPos + 1);
    
    assert(!_username.empty() && "Username cannot be empty");
    assert(!_password.empty() && "Password cannot be empty");
  }

}
