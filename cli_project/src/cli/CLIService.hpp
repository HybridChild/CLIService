#pragma once

#include "CLIServiceConfiguration.hpp"
#include "../user/User.hpp"
#include <memory>
#include <string>

class CLIService {
public:
  CLIService(std::unique_ptr<CLIServiceConfiguration> config);

  void activate();
  void service();
  bool isRunning() const { return _running; }

private:
  std::unique_ptr<CLIServiceConfiguration> _config;
  CommandMenuTree* _tree;
  InOutStream* _io;
  bool _running = false;
  bool _isAuthenticated = false;
  const User* _currentUser = nullptr;

  bool authenticateUser();
  void processCommand(const std::string& input);
  void handleNavigation(const CommandRequest& request);
  void handleExecution(const CommandRequest& request);
  bool navigateToNode(const std::vector<std::string>& path, bool isAbsolute);
  void executeCommand(const std::string& commandName, const CommandRequest& request);
  void printResponse(const CommandRequest& request);
  void printWelcomeMessage();
  void printGoodbyeMessage();
  void listCurrentCommands();
};
