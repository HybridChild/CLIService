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
  std::string responseString;

  if (_state == State::Stopped) {
    return;
  }

  std::string commandString = parseInputStream();
  if (commandString.empty()) {
    return;
  }

  if (_state == State::LoggedOut) {
    if (!authenticateUser(commandString)) {
      responseString = getLogInPrompt();
    }
    else {
      _state = State::LoggedIn;
      responseString = getWelcomeMessage();
    }
  }
  else if (_state == State::LoggedIn) {
    if (commandString == "exit") {
      _state = State::Stopped;
      responseString = getExitString();
    }
    else if (commandString == "help") {
      responseString = generateHelpString();
    }
    else {
      CommandRequest cmdRequest(commandString);
      processCommand(cmdRequest, responseString);
    }
  }

  if (_state != State::Stopped && _currentUser != nullptr) {
    responseString += getPromptString();
  }
  outputResponse(responseString);
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
    _tree->setCurrentNode(_tree->getRoot());
    response = "Navigated to root: /\n";
    return;
  }

  if (navigateToNode(request, response)) {
    response = "Current location: " + _tree->getCurrentPath() + "\n";
  }
}

bool CLIService::navigateToNode(const CommandRequest& request, std::string& response)
{
  if (request.isAbsolute()) {
    _tree->setCurrentNode(_tree->getRoot());
  }

  for (const auto& pathSegment : request.getPath()) {
    if (pathSegment == "..") {
      MenuNode* parent = _tree->getCurrentNode()->getParent();
      if (parent) {
        _tree->setCurrentNode(parent);
      } else {
        response = "Already at root.\n";
        return false;
      }
    }
    else {
      MenuNode* nextNode = _tree->getCurrentNode()->getSubMenu(pathSegment);
      if (!nextNode) {
        response = "Invalid path: " + pathSegment + "\n";
        return false;
      }
      else if(static_cast<int>(_currentUser->getAccessLevel()) < static_cast<int>(nextNode->getAccessLevel())) {
        response = "Access denied: " + pathSegment + "\n";
        return false;
      } else {
        _tree->setCurrentNode(nextNode);
      }
    }
  }
  return true;
}


void CLIService::handleExecution(const CommandRequest& request, std::string& response) {
  MenuNode* originalNode = _tree->getCurrentNode();

  if (navigateToNode(request, response)) {
    executeCommand(request, response);
  }

  _tree->setCurrentNode(originalNode);
}


void CLIService::executeCommand(const CommandRequest& request, std::string& response) {
  Command* cmd = _tree->getCurrentNode()->getCommand(request.getCommandName());
  
  if (!cmd) {
    response = "Unknown command. Use 'help' for available commands.\n";
  }
  else if (static_cast<int>(_currentUser->getAccessLevel()) < static_cast<int>(cmd->getAccessLevel())) {
    response = "Access denied. Your access level is too low for this command.\n";
  }
  else {
    _tree->processRequest(request, response);
  }
}

std::string CLIService::generateHelpString() {
  std::string helpString = "";
  helpString += "Current location: " + _tree->getCurrentPath() + "\n";
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

void CLIService::outputResponse(const std::string& response) {
  if (!response.empty()) {
    _config->getIOStream()->write(response);
  }
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
  return _currentUser->getUsername() + "@" + _tree->getCurrentPath() + " > ";
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

std::string CLIService::parseInputStream() {
  char c;
  while (std::cin.get(c)) {
    if (c == '\r' || c == '\n') {
      if (!_inputBuffer.empty()) {
        std::string command(_inputBuffer.begin(), _inputBuffer.end());
        _inputBuffer.clear();
        return command;
      }
    } else {
      _inputBuffer.push_back(c);
    }
  }
  return "";  // No complete command found
}
