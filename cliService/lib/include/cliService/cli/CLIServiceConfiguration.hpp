#pragma once
#include "cliService/user/User.hpp"
#include "cliService/tree/Directory.hpp"
#include "cliService/terminal/TerminalIf.hpp"
#include <vector>
#include <memory>

namespace cliService
{

  struct CLIServiceConfiguration
  {    
    CLIServiceConfiguration(
      TerminalIf& terminal,
      std::vector<User> users, 
      std::unique_ptr<Directory> root)
      : _terminal(terminal)
      , _users(std::move(users))
      , _root(std::move(root))
    {}

    TerminalIf& _terminal;
    std::vector<User> _users;
    std::unique_ptr<Directory> _root;
  };

}
