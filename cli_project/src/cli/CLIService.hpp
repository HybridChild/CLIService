#pragma once

#include "CLIServiceConfiguration.hpp"
#include "../user/User.hpp"
#include <memory>
#include <string>

class CLIService {
public:
  enum class State {
    Stopped,
    LoggedOut,
    LoggedIn
  };

  CLIService(std::unique_ptr<CLIServiceConfiguration> conf);

  void activate();
  void service();
  bool isRunning() const { return _state != State::Stopped; }

private:
  std::unique_ptr<CLIServiceConfiguration> _config;
  const User* _currentUser = nullptr;
  CommandMenuTree* _tree = nullptr;
  bool _isAuthenticated = false;
  std::deque<char> _inputBuffer;

  State _state = State::Stopped;

  std::string parseInputStream();
  bool authenticateUser(const std::string& commandString);
  const std::string& processCommand(CommandRequest& request);

  void handleNavigation(CommandRequest& request);
  bool navigateToNode(CommandRequest& request);
  void handleExecution(CommandRequest& request);
  void executeCommand(CommandRequest& request);
  
  void outputResponse(const std::string& response);
  
  std::string getLogInPrompt();
  std::string getWelcomeMessage();
  std::string generateHelpString();
  std::string getPromptString();
  std::string getExitString();
};
