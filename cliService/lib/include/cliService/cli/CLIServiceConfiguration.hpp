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
    TerminalIf& terminal;
    std::vector<User> users;
    std::unique_ptr<Directory> root;
    
    CLIServiceConfiguration(
      TerminalIf& term,
      std::vector<User> u, 
      std::unique_ptr<Directory> r)
      : terminal(term)
      , users(std::move(u))
      , root(std::move(r))
    {}
  };

}
