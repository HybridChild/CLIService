#include "cliService/cli/CLIService.hpp"
#include "cliService/cli/ActionRequest.hpp"
#include "cliService/cli/LoginRequest.hpp"
#include "cliService/cli/TerminalIf.hpp"
#include "cliService/cli/User.hpp"
#include "cliService/tree/CommandIf.hpp"
#include "cliService/tree/Directory.hpp"
#include <cassert>


namespace cliService
{

  const std::unordered_set<std::string_view> CLIService::GLOBAL_COMMANDS = {
    "logout",
    "key:tab", "key:up", "key:down", "key:left", "key:right"
  };


  CLIService::CLIService(CLIServiceConfiguration config)
    : _terminal(config._terminal)
    , _parser(_terminal, _currentState)
    , _users(std::move(config._users))
    , _root(std::move(config._root))
    , _currentDirectory(_root.get())
    , _currentUser(std::nullopt)
    , _currentState(CLIState::Inactive)
    , _pathResolver(*_root)
  {
    assert(!_users.empty() && "User list cannot be empty");
    assert(_root != nullptr && "Root directory cannot be null");
    assert(_currentDirectory != nullptr && "Current directory must be set");
  }


  void CLIService::activate() 
  {
    assert(_currentState == CLIState::Inactive && "Service must be inactive to activate");
    _currentState = CLIState::LoggedOut;
    _terminal.putString(WELCOME_MESSAGE);
    _terminal.putChar('\n');
    _terminal.putString(LOGGED_OUT_MESSAGE);
    _terminal.putChar('\n');
  }


  void CLIService::service() 
  {
    if (_currentState == CLIState::Inactive)
    {
      return;
    }

    auto request = _parser.service();
    if (!request)
    {
      return;
    }

    // Process state-specific requests
    switch (_currentState)
    {
      case CLIState::LoggedOut:
        if (auto* loginRequest = dynamic_cast<LoginRequest*>(request->get()))
        {
          handleLoginRequest(*loginRequest);
        }
        break;

      case CLIState::LoggedIn:
        if (auto* actionRequest = dynamic_cast<ActionRequest*>(request->get()))
        {
          handleActionRequest(*actionRequest);
        }
        break;

      default:
        assert(false && "Invalid state in service");
        break;
    }
  }


  void CLIService::handleLoginRequest(const LoginRequest& request) 
  {
    const auto& username = request.getUsername();
    const auto& password = request.getPassword();

    auto userIt = std::find_if(_users.begin(), _users.end(),
      [&](const User& user)
      {
        return user.getUsername() == username && user.getPassword() == password;
      });

    if (userIt != _users.end())
    {
      _currentUser = *userIt;
      _currentState = CLIState::LoggedIn;
      displayPrompt();
    }
    else
    {
      _terminal.putString("Invalid username or password\n");
    }
  }


  void CLIService::handleActionRequest(const ActionRequest& request) 
  {
    assert(_currentUser && "No user logged in");

    const auto& path = request.getPath();
    
    // Handle global commands
    if (!path.isEmpty() && GLOBAL_COMMANDS.find(path.elements().front()) != GLOBAL_COMMANDS.end())
    {
      handleGlobalCommand(path.elements().front(), request.getArgs());
      return;
    }

    // Resolve and validate path
    NodeIf* node = resolvePath(path);
    if (!node)
    {
      _terminal.putString("Invalid path\n");
      return;
    }

    if (!validatePathAccess(node))
    {
      _terminal.putString("Access denied\n");
      return;
    }

    // Handle directory navigation or command execution
    if (node->isDirectory())
    {
      _currentDirectory = static_cast<Directory*>(node);
      displayPrompt();
    }
    else
    {
      auto* cmd = static_cast<CommandIf*>(node);
      CommandResponse response = cmd->execute(request.getArgs());

      if (!response.getMessage().empty())
      {
        _terminal.putString(response.getMessage());
        _terminal.putChar('\n');
      }

      if (response.shouldShowPrompt())
      {
        displayPrompt();
      }
    }
  }


  void CLIService::handleGlobalCommand(const std::string_view& command, const std::vector<std::string>& args) 
  {
    if (command == "logout")
    {
      _currentState = CLIState::LoggedOut;
      _currentUser = std::nullopt;
      resetToRoot();
      _terminal.putString(LOGGED_OUT_MESSAGE);
      _terminal.putChar('\n');
    }
    else if (command == "key:tab")
    {
      _terminal.putString("Tab pressed\n");
      displayPrompt();
    }
    else if (command == "key:up")
    {
      _terminal.putString("Up arrow pressed\n");
      displayPrompt();
    }
    else if (command == "key:down")
    {
      _terminal.putString("Down arrow pressed\n");
      displayPrompt();
    }
    else if (command == "key:left")
    {
      _terminal.putString("Left arrow pressed\n");
      displayPrompt();
    }
    else if (command == "key:right")
    {
      _terminal.putString("Right arrow pressed\n");
      displayPrompt();
    }
  }

  NodeIf* CLIService::resolvePath(const Path& path) const 
  {
    return _pathResolver.resolve(path, *_currentDirectory);
  }

  bool CLIService::validatePathAccess(const NodeIf* node) const 
  {
    assert(_currentUser && "No user logged in");
    
    if (!node)
    {
      return false;
    }

    // Check access levels up the tree
    const NodeIf* current = node;
    while (current)
    {
      if (current->getAccessLevel() > _currentUser->getAccessLevel())
      {
        return false;
      }
      current = current->getParent();
    }
    
    return true;
  }


  void CLIService::resetToRoot() 
  {
    _currentDirectory = _root.get();
  }


  void CLIService::displayPrompt() const 
  {
    if (_currentState == CLIState::LoggedIn && _currentUser)
    {
      _terminal.putString(_currentUser->getUsername());
      _terminal.putString("@");
      _terminal.putString("> ");
    }
  }

}
