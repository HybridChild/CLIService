#pragma once
#include "CLIServiceConfiguration.hpp"
#include "cliService/cli/CLIState.hpp"
#include "cliService/parser/CommandParser.hpp"
#include "cliService/tree/Directory.hpp"
#include <optional>
#include <unordered_set>

namespace cliService
{

  class CLIService 
  {
  public:
    explicit CLIService(CLIServiceConfiguration config);
    
    void activate();
    void deactivate();
    void service();

  private:
    // Path resolution and validation
    NodeIf* resolvePath(const std::vector<std::string>& path, bool isAbsolute) const;
    bool validatePathAccess(const std::vector<std::string>& path, bool isAbsolute) const;
    
    // Request handlers
    void handleLoginRequest(const LoginRequest& request);
    void handleActionRequest(const ActionRequest& request);
    void handleGlobalCommand(const std::string& command, const std::vector<std::string>& args);
    
    // State management 
    void resetToRoot();
    void displayPrompt() const;
    
    TerminalIf& _terminal;
    CommandParser _parser;
    std::vector<User> _users;
    std::unique_ptr<Directory> _root;
    Directory* _currentDirectory;
    std::optional<User> _currentUser;
    CLIState _currentState;

    static const std::unordered_set<std::string_view> GLOBAL_COMMANDS;
    static constexpr std::string_view WELCOME_MESSAGE = "Welcome to CLI Service. Please login.";
    static constexpr std::string_view LOGOUT_MESSAGE = "Logged out. Please login.";
    static constexpr std::string_view INACTIVE_MESSAGE = "CLI Service inactive.";
  };

}
