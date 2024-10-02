#pragma once

#include "CLIServiceConfiguration.hpp"
#include <memory>
#include <string>

class CLIService {
public:
  CLIService(std::unique_ptr<CLIServiceConfiguration> conf);

  void activate();
  void service();
  void processCommand(const std::string& input);
  void listCurrentCommands();
  bool isRunning() const { return running; }

private:
  std::unique_ptr<CLIServiceConfiguration> config;
  CommandMenuTree* tree;
  InOutStream* io;
  bool running = false;

  void printResponse(const CommandRequest& request);
};
