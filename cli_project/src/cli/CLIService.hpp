#pragma once

#include "CLIServiceConfiguration.hpp"
#include <memory>
#include <string>

class CLIService {
public:
  CLIService(std::unique_ptr<CLIServiceConfiguration> conf);

  void activate();
  void service();
  bool isRunning() const { return running; }


private:
  std::unique_ptr<CLIServiceConfiguration> config;
  CommandMenuTree* tree;
  InOutStream* io;
  bool running = false;
  bool isAuthenticated = false;
  std::string currentUser;

  void processCommand(const std::string& input);
  void printResponse(const CommandRequest& request);
  void listCurrentCommands();
  void printWelcomeMessage();
  void printGoodbyeMessage();
  bool authenticateUser();
};
