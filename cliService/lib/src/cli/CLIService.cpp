#include "cliService/cli/CLIService.hpp"
#include "cliService/terminal/TerminalIf.hpp"
#include "cliService/tree/Directory.hpp"
#include "cliService/tree/CommandIf.hpp"
#include "cliService/requests/LoginRequest.hpp"
#include "cliService/requests/ActionRequest.hpp"
#include "cliService/user/User.hpp"
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
    
    // First check global commands
    if (!path.empty() && GLOBAL_COMMANDS.find(path[0]) != GLOBAL_COMMANDS.end())
    {
      handleGlobalCommand(path[0], request.getArgs());
      return;
    }

    // Then try to resolve path
    if (NodeIf* node = resolvePath(path, request.isAbsolutePath()))
    {
      if (!validatePathAccess(path, request.isAbsolutePath()))
      {
        _terminal.putString("Access denied\n");
        return;
      }

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
    else
    {
      _terminal.putString("Invalid path\n");
    }
  }


  void CLIService::handleGlobalCommand(const std::string_view& command, const std::vector<std::string>& args) 
  {
    if (command == "logout")
    {
      _currentState = CLIState::LoggedOut;
      _currentUser = std::nullopt;
      resetToRoot();
      _terminal.putString(LOGOUT_MESSAGE);
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


  NodeIf* CLIService::resolvePath(const std::vector<std::string>& path, bool isAbsolute) const 
  {
    if (path.empty())
    {
      return _currentDirectory;
    }

    return isAbsolute ? _root->findNode(path) : _currentDirectory->findNode(path);
  }


  bool CLIService::validatePathAccess(const std::vector<std::string>& path, bool isAbsolute) const 
  {
    assert(_currentUser && "No user logged in");
    
    NodeIf* current = isAbsolute ? _root.get() : _currentDirectory;
    
    // For absolute paths, start with root
    if (isAbsolute)
    {
      if (current->getAccessLevel() > _currentUser->getAccessLevel())
      {
        return false;
      }
    }
    
    // Check each component of the path
    for (const auto& component : path)
    {
      auto dir = static_cast<Directory*>(current);
      NodeIf* next = dir->findNode({component});
      if (!next || next->getAccessLevel() > _currentUser->getAccessLevel())
      {
        return false;
      }
      current = next;
    }
    
    return true;
  }


  void CLIService::resetToRoot() 
  {
    _currentDirectory = _root.get();
  }
  

  void CLIService::displayPrompt() const 
  {
    if (_currentState == CLIState::LoggedIn)
    {
      _terminal.putString(_currentUser->getUsername());
      _terminal.putString("@");
      _terminal.putString("> ");
    }
  }

}
