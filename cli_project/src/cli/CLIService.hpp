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

  void processCommand(const CommandRequest& request, std::string& response);
  void handleNavigation(const CommandRequest& request, std::string& response);
  bool navigateToNode(const CommandRequest& request, std::string& response);
  void handleExecution(const CommandRequest& request, std::string& response);
  void executeCommand(const CommandRequest& request, std::string& response);

  bool validateAccessLevel(const Command& command);
  bool validateAccessLevel(const MenuNode& node);

  void outputResponse(const std::string& response);

  std::string getLogInPrompt();
  std::string getWelcomeMessage();
  std::string generateHelpString();
  std::string getPromptString();
  std::string getExitString();
};
