#include "cliService/cli/CLIService.hpp"
#include "cliService/cli/ActionRequest.hpp"
#include "cliService/cli/LoginRequest.hpp"
#include "cliService/cli/TerminalIf.hpp"
#include "cliService/cli/User.hpp"
#include "cliService/tree/CommandIf.hpp"
#include "cliService/tree/Directory.hpp"
#include "cliService/tree/PathCompleter.hpp"
#include <cassert>


namespace cliService
{

  const std::unordered_set<std::string_view> CLIService::GLOBAL_COMMANDS = {
    "logout", "tree", "help", "?"
  };


  CLIService::CLIService(CLIServiceConfiguration config)
    : _terminal(config._terminal)
    , _parser(_terminal, _currentState)
    , _users(std::move(config._users))
    , _rootDirectory(std::move(config._rootDirectory))
    , _currentDirectory(_rootDirectory.get())
    , _currentUser(std::nullopt)
    , _currentState(CLIState::Inactive)
    , _pathResolver(*_rootDirectory)
  {
    assert(!_users.empty() && "User list cannot be empty");
    assert(_rootDirectory != nullptr && "Root directory cannot be null");
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
    displayPrompt();
  }


  void CLIService::service() 
  {
    if (_currentState == CLIState::Inactive) { return; }

    auto request = _parser.parseNextRequest();
    if (!request) { return; }

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

    if (request.getTrigger() != ActionRequest::Trigger::Enter)
    {
      handleSpecialKey(request);
      return;
    }

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
      displayPrompt();
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
      if (!args.empty())
      {
        _terminal.putString(NO_ARGUMENTS_MESSAGE);
        _terminal.putChar('\n');
        displayPrompt();
        return;
      }

      _currentState = CLIState::LoggedOut;
      _currentUser = std::nullopt;
      resetToRoot();
      _terminal.putString(LOGGED_OUT_MESSAGE);
      _terminal.putChar('\n');
      displayPrompt();
    }
    else if (command == "tree")
    {
      
      if (!args.empty())
      {
        _terminal.putString(NO_ARGUMENTS_MESSAGE);
        _terminal.putChar('\n');
        displayPrompt();
        return;
      }

      // print tree structure string
      _terminal.putChar('\n');
      _currentDirectory->traverse([&](const NodeIf& node, int depth) {
        std::string indent(depth * 2, ' ');
        std::string treeStr = indent + node.getName() + (node.isDirectory() ? "/" : "") + "\n";
        _terminal.putString(treeStr);
      });

      _terminal.putChar('\n');
      displayPrompt();
    }
    else if (command == "?")
    {
      if (!args.empty())
      {
        _terminal.putString(NO_ARGUMENTS_MESSAGE);
        _terminal.putChar('\n');
        displayPrompt();
        return;
      }

      _terminal.putChar('\n');
      _terminal.putString("Available commands in current directory:\n");
      _currentDirectory->traverse([&](const NodeIf& node, int depth) {
        // Only show items in current directory and with appropriate access level
        if (depth == 1 && node.getAccessLevel() <= _currentUser->getAccessLevel())
        {
          std::string indent("  ");
          std::string entry = indent + node.getName();
          
          if (node.isDirectory())
          {
            entry += "/";
          }
          else if (auto* cmd = dynamic_cast<const CommandIf*>(&node))
          {
            if (!cmd->getDescription().empty())
            {
              entry += " - " + cmd->getDescription();
            }
          }
          entry += "\n";
          _terminal.putString(entry);
        }
      });

      _terminal.putChar('\n');
      displayPrompt();
    }
    else if (command == "help")
    {
      if (!args.empty())
      {
        _terminal.putString(NO_ARGUMENTS_MESSAGE);
        _terminal.putChar('\n');
        displayPrompt();
        return;
      }

      // Show global commands
      _terminal.putString("\nGlobal commands:\n");
      _terminal.putString("  help   - List global commands\n");
      _terminal.putString("  tree   - Show directory structure\n");
      _terminal.putString("  ?      - Show help for available commands in current directory\n");
      _terminal.putString("  logout - Exit current session\n\n");

      displayPrompt();
    }
  }
  

  void CLIService::handleSpecialKey(const ActionRequest& request)
  {
    switch (request.getTrigger())
    {
    case ActionRequest::Trigger::Tab:
      handleTabCompletion(request);
      break;
    case ActionRequest::Trigger::ArrowLeft:
    case ActionRequest::Trigger::ArrowRight:
    default:
      break;
    };
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


  void CLIService::handleTabCompletion(const ActionRequest& request)
  {
    auto currentInput = _parser.getBuffer();
    auto node = resolvePath(request.getPath());

    if (node && node->isDirectory() && !currentInput.empty() && currentInput.back() != '/')
    {
      _terminal.putChar('/');
      _parser.appendToBuffer("/");
      return;
    }

    auto result = PathCompleter::complete(*_currentDirectory, currentInput, _currentUser->getAccessLevel());

    if (!result.fillCharacters.empty())
    {
      _terminal.putString(result.fillCharacters);
      _parser.appendToBuffer(result.fillCharacters);

      if (result.isDirectory)
      {
        _terminal.putChar('/');
        _parser.appendToBuffer("/");
      }
    }
    else if (!result.allOptions.empty())
    {
      _terminal.putChar('\n');
      for (const auto& opt : result.allOptions)
      {
        _terminal.putString(opt + "   ");
      }

      _terminal.putChar('\n');
      displayPrompt();
      _terminal.putString(_parser.getBuffer());
    }
  }


  void CLIService::resetToRoot() 
  {
    _currentDirectory = _rootDirectory.get();
  }


  void CLIService::displayPrompt() const 
  {
    if (_currentState == CLIState::LoggedIn && _currentUser)
    {
      _terminal.putString(_currentUser->getUsername());
      _terminal.putString("@");
      std::string pathStr = _pathResolver.getAbsolutePath(*_currentDirectory).toString();
      _terminal.putString(pathStr);
    }

    _terminal.putString(" > ");
  }

}
