#pragma once

#include "CLIServiceConfiguration.hpp"
#include <memory>
#include <string>

class CLIService {
public:
  CLIService(std::unique_ptr<CLIServiceConfiguration> config);

  void processCommand(const std::string& input);
  void listCurrentCommands();
  void run();

private:
  std::unique_ptr<CLIServiceConfiguration> configuration;
  void printResponse(const CommandRequest& request);
};
