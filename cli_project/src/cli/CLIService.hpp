#pragma once

#include "CLIServiceConfiguration.hpp"
#include <memory>
#include <string>

class CLIService {
public:
  CLIService(std::unique_ptr<CLIServiceConfiguration> config);

  void activate();
  void service();
  void processCommand(const std::string& input);
  void listCurrentCommands();
  bool isRunning() const { return running; }

private:
  std::unique_ptr<CLIServiceConfiguration> configuration;
  bool running = false;

  void printResponse(const CommandRequest& request);
};
