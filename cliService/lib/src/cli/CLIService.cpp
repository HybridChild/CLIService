#include "cliService/cli/CLIService.hpp"
#include "cliService/cli/ActionRequest.hpp"
#include "cliService/cli/LoginRequest.hpp"
#include "cliService/cli/CharIOStreamIf.hpp"
#include "cliService/cli/User.hpp"
#include "cliService/tree/CommandIf.hpp"
#include "cliService/tree/Directory.hpp"
#include "cliService/tree/PathCompleter.hpp"
#include <cassert>


namespace cliService
{

  const std::unordered_map<std::string_view, CLIService::GlobalCommandHandler> CLIService::GLOBAL_COMMAND_HANDLERS = {
    {"logout" , &CLIService::handleGlobalLogout},
    {"exit"   , &CLIService::handleGlobalExit},
    {"tree"   , &CLIService::handleGlobalTree},
    {"help"   , &CLIService::handleGlobalHelp},
    {"?"      , &CLIService::handleGlobalQuestionMark},
    {"clear"  , &CLIService::handleGlobalClear}
  };

  CLIService::CLIService(CLIServiceConfiguration config)
    : _ioStream(config._ioStream)
    , _parser(_ioStream, _currentState, config._historySize)
    , _users(std::move(config._users))
    , _currentUser(std::nullopt)
    , _rootDirectory(std::move(config._rootDirectory))
    , _currentDirectory(_rootDirectory.get())
    , _pathResolver(*_rootDirectory)
    , _currentState(CLIState::Inactive)
  {
    assert(!_users.empty() && "User list cannot be empty");
    assert(_rootDirectory != nullptr && "Root directory cannot be null");
    assert(_currentDirectory != nullptr && "Current directory must be set");
  }


  void CLIService::activate()
  {
    assert(_currentState == CLIState::Inactive && "Service must be inactive to activate");
    _currentState = CLIState::LoggedOut;
    _ioStream.putString(WELCOME_MESSAGE);
    displayNewLine();
    _ioStream.putString(LOGGED_OUT_MESSAGE);
    displayNewLine();
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
      if (auto* loginRequest = dynamic_cast<LoginRequest*>(request->get())) {
        handleLoginRequest(*loginRequest);
      }
      else if (dynamic_cast<InvalidLoginRequest*>(request->get())) {
        handleInvalidLoginRequest();
      }
      break;

    case CLIState::LoggedIn:
      if (auto* actionRequest = dynamic_cast<ActionRequest*>(request->get())) {
        handleActionRequest(*actionRequest);
      }
      break;

    default:
      assert(false && "Invalid state in service");
      break;
    }
  }


  NodeIf* CLIService::resolvePath(const Path& path) const {
    return _pathResolver.resolve(path, *_currentDirectory);
  }


  bool CLIService::validatePathAccess(const NodeIf* node) const
  {
    assert(_currentUser && "No user logged in");
    
    if (!node) {
      return false;
    }

    // Check access levels up the tree
    const NodeIf* current = node;

    while (current)
    {
      if (current->getAccessLevel() > _currentUser->getAccessLevel()) {
        return false;
      }

      current = current->getParent();
    }
    
    return true;
  }


  void CLIService::resetToRoot() {
    _currentDirectory = _rootDirectory.get();
  }


  void CLIService::displayPrompt() const
  {
    if (_currentState == CLIState::LoggedIn && _currentUser)
    {
      _ioStream.putString(_currentUser->getUsername());
      _ioStream.putString("@");
      std::string pathStr = _pathResolver.getAbsolutePath(*_currentDirectory).toString();
      _ioStream.putString(pathStr);
    }

    _ioStream.putString(" > ");
  }


  void CLIService::displayNewLine(uint32_t number) const
  {
    for (uint32_t i = 0; i < number; ++i)
    {
      _ioStream.putChar('\r');
      _ioStream.putChar('\n');
    }
  }


  void CLIService::handleInvalidLoginRequest()
  {
    _ioStream.putString(LOGGED_OUT_MESSAGE);
    displayNewLine();
    displayPrompt();
  }


  void CLIService::handleLoginRequest(const LoginRequest& request)
  {
    const auto& username = request.getUsername();
    const auto& password = request.getPassword();

    auto userIt = std::find_if(_users.begin(), _users.end(),
      [&](const User& user) {
        return user.getUsername() == username && user.getPassword() == password;
      });

    if (userIt != _users.end())
    {
      _currentUser = *userIt;
      _currentState = CLIState::LoggedIn;
    }
    else {
      _ioStream.putString("Invalid username or password\r\n");
    }

    displayPrompt();
  }


  void CLIService::handleActionRequest(const ActionRequest& request) 
  {
    assert(_currentUser && "No user logged in");

    // Handle special keys
    if (request.getTrigger() != ActionRequest::Trigger::Enter)
    {
      handleSpecialKey(request);
      return;
    }

    const auto& path = request.getPath();
    
    if (!path.isEmpty())
    {
      const auto& command = path.elements().front();

      if (GLOBAL_COMMAND_HANDLERS.find(command) != GLOBAL_COMMAND_HANDLERS.end())
      {
        handleGlobalCommand(command, request.getArgs());
        return;
      }
    }

    // Resolve and validate path
    NodeIf* node = resolvePath(path);
    if (!node)
    {
      _ioStream.putString("Invalid path\r\n");
      displayPrompt();
      return;
    }

    if (!validatePathAccess(node))
    {
      _ioStream.putString("Access denied\r\n");
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
        _ioStream.putString(response.getMessage());
        displayNewLine();
      }

      if (response.shouldShowPrompt())
      {
        displayPrompt();
      }
    }
  }


  void CLIService::handleGlobalCommand(const std::string_view& command, const std::vector<std::string>& args)
  {
    auto it = GLOBAL_COMMAND_HANDLERS.find(command);

    if (it != GLOBAL_COMMAND_HANDLERS.end()) {
      (this->*(it->second))(args);
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


  void CLIService::handleTabCompletion(const ActionRequest& request)
  {
    auto currentInput = _parser.getBuffer();
    auto node = resolvePath(request.getPath());

    if (node && node->isDirectory() && !currentInput.empty() && currentInput.back() != '/')
    {
      _ioStream.putChar('/');
      _parser.appendToBuffer("/");
      return;
    }

    auto result = PathCompleter::complete(*_currentDirectory, currentInput, _currentUser->getAccessLevel());

    if (!result.fillCharacters.empty())
    {
      _ioStream.putString(result.fillCharacters);
      _parser.appendToBuffer(result.fillCharacters);

      if (result.isDirectory)
      {
        _ioStream.putChar('/');
        _parser.appendToBuffer("/");
      }
    }
    else if (!result.allOptions.empty())
    {
      displayNewLine();

      for (const auto& opt : result.allOptions) {
        _ioStream.putString(opt + "   ");
      }

      displayNewLine();
      displayPrompt();
      _ioStream.putString(_parser.getBuffer());
    }
  }


  void CLIService::handleGlobalHelp(const std::vector<std::string>& args)
  {
    if (!args.empty())
    {
      _ioStream.putString(NO_ARGUMENTS_MESSAGE);
      displayNewLine();
      displayPrompt();
      return;
    }

    // List global commands
    displayNewLine();
    _ioStream.putString("\thelp   - List global commands\r\n");
    _ioStream.putString("\ttree   - Print directory tree\r\n");
    _ioStream.putString("\t?      - Show description of available commands in current directory\r\n");
    _ioStream.putString("\tlogout - Exit current session\r\n");
    _ioStream.putString("\tclear  - Clear screen\r\n");
    _ioStream.putString("\texit   - Exit the CLI\r\n");
    displayNewLine();

    displayPrompt();
  }

  void CLIService::handleGlobalTree(const std::vector<std::string>& args)
  {
    if (!args.empty())
    {
      _ioStream.putString(NO_ARGUMENTS_MESSAGE);
      displayNewLine();
      displayPrompt();
      return;
    }

    displayNewLine();

    _currentDirectory->traverse([&](const NodeIf& node, size_t depth) {
      if (node.getAccessLevel() <= _currentUser->getAccessLevel()) {
        std::string indent(depth * 2, ' ');
        std::string treeStr = indent + node.getName() + (node.isDirectory() ? "/" : "");
        _ioStream.putString(treeStr);
        displayNewLine();
      }
    });

    displayNewLine();
    displayPrompt();
  }


  void CLIService::handleGlobalQuestionMark(const std::vector<std::string>& args)
  {
    if (!args.empty())
    {
      _ioStream.putString(NO_ARGUMENTS_MESSAGE);
      displayNewLine();
      displayPrompt();
      return;
    }

    displayNewLine();

    _currentDirectory->traverse([&](const NodeIf& node, size_t depth) {
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
          if (!cmd->getDescription().empty()) {
            entry += " - " + cmd->getDescription();
          }
        }

        _ioStream.putString(entry);
        displayNewLine();
      }
    });

    displayNewLine();
    displayPrompt();
  }


  void CLIService::handleGlobalLogout(const std::vector<std::string>& args)
  {
    if (!args.empty())
    {
      _ioStream.putString(NO_ARGUMENTS_MESSAGE);
      displayNewLine();
      displayPrompt();
      return;
    }

    _currentState = CLIState::LoggedOut;
    _currentUser = std::nullopt;
    resetToRoot();
    _ioStream.putString(LOGGED_OUT_MESSAGE);
    displayNewLine();
    displayPrompt();
  }

  void CLIService::handleGlobalClear(const std::vector<std::string>& args)
  {
    if (!args.empty())
    {
      _ioStream.putString(NO_ARGUMENTS_MESSAGE);
      displayNewLine();
      displayPrompt();
      return;
    }

    // Send ANSI escape sequence to clear screen and move cursor to home position
    _ioStream.putString("\033[2J");
    _ioStream.putString("\033[H");
    displayPrompt();
  }


  void CLIService::handleGlobalExit(const std::vector<std::string>& args)
  {
    if (!args.empty())
    {
      _ioStream.putString(NO_ARGUMENTS_MESSAGE);
      displayNewLine();
      displayPrompt();
      return;
    }

    _currentState = CLIState::Inactive;
    _currentUser = std::nullopt;
    resetToRoot();
    _ioStream.putString(EXIT_MESSAGE);
    displayNewLine();
  }

}
