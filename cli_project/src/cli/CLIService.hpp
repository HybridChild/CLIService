#pragma once

#include <memory>
#include <string>
#include <deque>

#include "CLIServiceConfiguration.hpp"
#include "../user/User.hpp"

namespace cliService {

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
    std::deque<char> _inputBuffer;
    State _state = State::Stopped;
    const User* _currentUser = nullptr;
    CommandMenuTree* _tree = nullptr;
    IOStreamIf* _io = nullptr;

    void parseInputStream(std::string& cmdString);
    bool authenticateUser(const std::string& commandString);

    void processCommand(const CommandRequest& request, std::string& response);
    void handleNavigation(const CommandRequest& request, std::string& response);
    void handleExecution(const CommandRequest& request, std::string& response);

    bool validateAccessLevel(const Command& command);
    bool validateAccessLevel(const MenuNode& node);

    void outputResponse(const std::string& response);

    std::string generateHelpString();
    std::string generatePromptString();
  };

}
