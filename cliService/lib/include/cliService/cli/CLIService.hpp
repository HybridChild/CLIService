// CLIService.hpp
#pragma once
#include "cliService/cli/CLIServiceConfiguration.hpp"
#include "cliService/cli/CLIState.hpp"
#include "cliService/cli/InputParser.hpp"
#include "cliService/tree/Directory.hpp"
#include "cliService/tree/Path.hpp"
#include "cliService/tree/PathResolver.hpp"
#include <optional>
#include <unordered_set>

namespace cliService
{

  class CLIService 
  {
  public:
    explicit CLIService(CLIServiceConfiguration config);
    
    void activate();
    void service();

  private:
    // Request handlers
    void handleLoginRequest(const LoginRequest& request);
    void handleActionRequest(const ActionRequest& request);
    void handleGlobalCommand(const std::string_view& command, const std::vector<std::string>& args);
    
    // Path operations
    NodeIf* resolvePath(const Path& path) const;
    bool validatePathAccess(const NodeIf* node) const;
    
    // State management 
    void resetToRoot();
    void displayPrompt() const;
    
    TerminalIf& _terminal;
    InputParser _parser;
    std::vector<User> _users;
    std::unique_ptr<Directory> _root;
    Directory* _currentDirectory;
    std::optional<User> _currentUser;
    CLIState _currentState;
    PathResolver _pathResolver;

    static const std::unordered_set<std::string_view> GLOBAL_COMMANDS;
    static constexpr std::string_view WELCOME_MESSAGE = "Welcome to CLI Service. Please login. <username>:<password>";
    static constexpr std::string_view LOGOUT_MESSAGE = "Logged out. Please enter <username>:<password>";
    static constexpr std::string_view INACTIVE_MESSAGE = "CLI Service inactive.";
  };

}
