#include "CLIService.hpp"
#include <iostream>

CLIService::CLIService(std::unique_ptr<CLIServiceConfiguration> conf) 
  : _config(std::move(conf))
{
  _tree = _config->getMenuTree();
}


void CLIService::activate() {
  _state = State::LoggedOut;
  outputResponse(getLogInPrompt());
}


void CLIService::service() {
  std::string commandString{};
  if (!parseInputStream(commandString)) {
    return;
  }

  std::string response;

  if (_state == State::LoggedOut) {
    if (authenticateUser(commandString)) {
      _state = State::LoggedIn;
      response = getWelcomeMessage();
    }
    else {
      response = "Invalid username or password\n";
      response += getLogInPrompt();
    }
  }
  else if (_state == State::LoggedIn) {
    if (commandString == "exit") {
      _state = State::Stopped;
      response = getExitString();
    }
    else if (commandString == "?") {
      response = generateHelpString();
    }
    else {
      CommandRequest cmdRequest(commandString);
      processCommand(cmdRequest, response);
    }
  }

  if (_state != State::Stopped && _currentUser != nullptr) {
    response += getPromptString();
  }
  
  outputResponse(response);
}


bool CLIService::parseInputStream(std::string& cmdString) {
  char c;

  while (_config->getIOStream()->getCharTimeout(c, 1)) {
    if (c == '\r' || c == '\n') {
      if (!_inputBuffer.empty()) {
        cmdString.assign(_inputBuffer.begin(), _inputBuffer.end());
        _inputBuffer.clear();
        return true;
      }
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

    if (!_tree->navigateToNode(request.getPath(), request.isAbsolute())) {
      response = "Invalid path: " + request.getPath().back() + "\n";
    }
    else if (validateAccessLevel(*_tree->getCurrentNode())) {
      response = "Access denied\n";
      _tree->setCurrentNode(originalNode);
    }
  }
}


void CLIService::handleExecution(const CommandRequest& request, std::string& response) {
  MenuNode* originalNode = _tree->getCurrentNode();
  if (!_tree->navigateToNode(request.getPath(), request.isAbsolute())) {
    response = "Invalid path: " + request.getPath().back() + "\n";
    return;
  }

  Command* cmd = _tree->getCurrentNode()->getCommand(request.getCommandName());
  if (cmd) {
    if (validateAccessLevel(*cmd)) {
      response = "Access denied\n";
    }
    else {
      cmd->execute(request, response);
    }

    _tree->setCurrentNode(originalNode);  // Reset to original position
  }
  else {
    response = "Invalid command: " + request.getCommandName() + "\n";
  }

  _tree->setCurrentNode(originalNode);
}


bool CLIService::validateAccessLevel(const Command& command) {
  return static_cast<int>(_currentUser->getAccessLevel()) < static_cast<int>(command.getAccessLevel());
}


bool CLIService::validateAccessLevel(const MenuNode& node) {
  return static_cast<int>(_currentUser->getAccessLevel()) < static_cast<int>(node.getAccessLevel());
}


void CLIService::outputResponse(const std::string& response) {
  if (!response.empty()) {
    _config->getIOStream()->putString(response);
  }
}


std::string CLIService::generateHelpString() {
  std::string helpString = "";
  helpString += "Current location: " + _tree->getPath(_tree->getCurrentNode()) + "\n";
  helpString += "Available commands:\n";
  for (const auto& [name, cmd] : _tree->getCurrentNode()->getCommands()) {
    if (static_cast<int>(_currentUser->getAccessLevel()) >= static_cast<int>(cmd->getAccessLevel())) {
      helpString += "  " + name + " - Usage: " + cmd->getUsage() + "\n";
    }
  }
  helpString += "Available submenus:\n";
  for (const auto& [name, submenu] : _tree->getCurrentNode()->getSubMenus()) {
    if (static_cast<int>(_currentUser->getAccessLevel()) >= static_cast<int>(submenu->getAccessLevel())) {
      helpString += "  " + name + "/" + "\n";
    }
  }
  return helpString;
}

std::string CLIService::getLogInPrompt() {
  return "Logged out. Please enter <username>:<password>\n > ";
}

std::string CLIService::getWelcomeMessage() {
  return "Welcome to the CLI Service. Type 'exit' to quit.\n";
}

std::string CLIService::getExitString() {
  return "Thank you for using the CLI Service. Goodbye!\n";
}

std::string CLIService::getPromptString() {
  return _currentUser->getUsername() + "@" + _tree->getPath(_tree->getCurrentNode()) + " > ";
}
