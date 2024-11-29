#include <iostream>
#include "cliService/CLIService.hpp"

namespace cliService {

  CLIService::CLIService(std::unique_ptr<CLIServiceConfiguration> conf) 
    : _config(std::move(conf))
  {
    _tree = _config->getMenuTree();
    _io = _config->getIOStream();
  }


  void CLIService::activate() {
    _state = State::LoggedOut;
    _io->putString(_config->getLogInPrompt());
  }


  void CLIService::service() {
    std::string commandString{};
    if (!parseInputStream(commandString)) {
      return;
    }

    std::string response{};

    switch (_state) {
      case State::LoggedOut:
        if (authenticateUser(commandString)) {
          _state = State::LoggedIn;
          response = _config->getWelcomeMessage();
          response += generatePromptString();
        }
        else {
          response = "Invalid username or password\n";
          response += _config->getLogInPrompt();
        }
        break;

      case State::LoggedIn:
        if (commandString.empty()) {
          response = "";
        }
        else if (commandString == "logout") {
          _state = State::LoggedOut;
          _currentUser = nullptr;
          response = _config->getLogInPrompt();
        }
        else if (commandString == "exit") {
          _state = State::Stopped;
          _currentUser = nullptr;
          response = _config->getExitString();
        }
        else if (commandString == "?") {
          response = generateHelpString();
        }
        else {
          CommandRequest cmdRequest(commandString);
          processCommand(cmdRequest, response);
        }

        response += generatePromptString();
        break;

      case State::Stopped:
      default:
        break;
    }
    
    outputResponse(response);
  }


  bool CLIService::parseInputStream(std::string& cmdString) {
    char c;

    while (_io->getCharTimeout(c, 1)) {
      if (c == '\r' || c == '\n') {
        if (!_inputBuffer.empty()) {
          cmdString.assign(_inputBuffer.begin(), _inputBuffer.end());
          _inputBuffer.clear();
        }
        return true;
      }
      else if (c == '\b' || c == 127) {  // Handle backspace and delete
        if (!_inputBuffer.empty()) {
          _inputBuffer.pop_back();
        }
      }
      else if (c >= 32 && c <= 126) {  // Printable ASCII characters
        _inputBuffer.push_back(c);
      }
    }

    return false;
  }


  bool CLIService::authenticateUser(const std::string& commandString) {
    size_t colonPos = commandString.find(':');

    if (colonPos == std::string::npos) {
      return false;
    }

    std::string username = commandString.substr(0, colonPos);
    std::string password = commandString.substr(colonPos + 1);

    _currentUser = _config->authenticateUser(username, password);
    return _currentUser != nullptr;
  }


  void CLIService::processCommand(const CommandRequest& cmdRequest, std::string& response) {
    switch(cmdRequest.getType())
    {
      case CommandRequest::Type::RootNavigation:
      case CommandRequest::Type::Navigation:
        handleNavigation(cmdRequest, response);
        break;
      case CommandRequest::Type::Execution:
        handleExecution(cmdRequest, response);
        break;
      default:
        break;
    }
  }


  void CLIService::handleNavigation(const CommandRequest& request, std::string& response) {
    if (request.getType() == CommandRequest::Type::RootNavigation) {
      _tree->returnToRoot();
    }
    else if (request.getType() == CommandRequest::Type::Navigation) {
      MenuNode* originalNode = _tree->getCurrentNode();
      bool success = _tree->navigateToNode(request.getPath(), request.isAbsolute());

      if (!success) {
        response = "Invalid path\n";
        return;
      }

      if (validateAccessLevel(*_tree->getCurrentNode())) {
        response = "Access denied\n";
        _tree->setCurrentNode(originalNode);
      }
    }
  }


  void CLIService::handleExecution(const CommandRequest& request, std::string& response) {
    MenuNode* originalNode = _tree->getCurrentNode();
    bool success = _tree->navigateToNode(request.getPath(), request.isAbsolute());

    if (!success) {
      response = "Invalid path\n";
      return;
    }

    CommandIf* cmd = _tree->getCurrentNode()->getCommand(request.getCommandName());

    if (cmd) {
      if (validateAccessLevel(*cmd)) {
        response = "Access denied\n";
      }
      else {
        cmd->execute(request, response);
      }
    }
    else {
      response = "Invalid command: " + request.getCommandName() + "\n";
    }

    _tree->setCurrentNode(originalNode);
  }


  bool CLIService::validateAccessLevel(const CommandIf& command) {
    return static_cast<uint32_t>(_currentUser->getAccessLevel()) < static_cast<uint32_t>(command.getAccessLevel());
  }


  bool CLIService::validateAccessLevel(const MenuNode& node) {
    return static_cast<uint32_t>(_currentUser->getAccessLevel()) < static_cast<uint32_t>(node.getAccessLevel());
  }


  void CLIService::outputResponse(const std::string& response) {
    if (!response.empty()) {
      _io->putString(response);
    }
  }


  std::string CLIService::generateHelpString() {
    std::string helpString{};
    helpString += _config->getIndent(1) + "Current location: " + _tree->getPath(_tree->getCurrentNode()) + "\n";
    helpString += _config->getIndent(1) + "Available commands:\n";

    for (const auto& [name, cmd] : _tree->getCurrentNode()->getCommands()) {
      if (static_cast<uint32_t>(_currentUser->getAccessLevel()) >= static_cast<uint32_t>(cmd->getAccessLevel())) {
        helpString += _config->getIndent(2) + name + " - Usage: " + cmd->getUsage() + "\n";
      }
    }

    helpString += _config->getIndent(1) + "Available submenus:\n";

    for (const auto& [name, submenu] : _tree->getCurrentNode()->getSubMenus()) {
      if (static_cast<uint32_t>(_currentUser->getAccessLevel()) >= static_cast<uint32_t>(submenu->getAccessLevel())) {
        helpString += _config->getIndent(2) + name + "/" + "\n";
      }
    }

    return helpString;
  }

  std::string CLIService::generatePromptString() {
    if (_currentUser == nullptr) {
      return "";
    }

    return _currentUser->getUsername() + "@" + _tree->getPath(_tree->getCurrentNode()) + " > ";
  }

}
