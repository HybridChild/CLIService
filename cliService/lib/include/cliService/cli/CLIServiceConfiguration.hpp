#pragma once
#include "cliService/cli/User.hpp"
#include "cliService/cli/TerminalIf.hpp"
#include "cliService/tree/Directory.hpp"
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
      , _rootDirectory(std::move(root))
    {}

    TerminalIf& _terminal;
    std::vector<User> _users;
    std::unique_ptr<Directory> _rootDirectory;
  };

}
