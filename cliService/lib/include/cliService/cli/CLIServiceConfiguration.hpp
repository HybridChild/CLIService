#pragma once
#include "cliService/cli/User.hpp"
#include "cliService/cli/CharIOStreamIf.hpp"
#include "cliService/cli/CLIMessages.hpp"
#include "cliService/tree/Directory.hpp"
#include <vector>
#include <memory>

namespace cliService
{

  struct CLIServiceConfiguration
  {
    CLIServiceConfiguration(
      CharIOStreamIf& ioStream,
      std::vector<User> users,
      std::unique_ptr<Directory> root,
      size_t historySize,
      CLIMessages messages = CLIMessages::getDefaults())
      : _ioStream(ioStream)
      , _users(std::move(users))
      , _rootDirectory(std::move(root))
      , _historySize(historySize)
      , _messages(std::move(messages))
    {}

    CharIOStreamIf& _ioStream;
    std::vector<User> _users;
    std::unique_ptr<Directory> _rootDirectory;
    size_t _historySize;
    CLIMessages _messages;
  };

}
