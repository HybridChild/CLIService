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
      uint32_t inputTimeout_ms,
      size_t historySize,
      CLIMessages messages = CLIMessages::getDefaults())
      : _ioStream(ioStream)
      , _users(std::move(users))
      , _rootDirectory(std::move(root))
      , _inputTimeout_ms(inputTimeout_ms)
      , _historySize(historySize)
      , _messages(std::move(messages))
    {}

    CharIOStreamIf& _ioStream;
    std::vector<User> _users;
    std::unique_ptr<Directory> _rootDirectory;
    uint32_t _inputTimeout_ms;
    size_t _historySize;
    CLIMessages _messages;
  };

}
