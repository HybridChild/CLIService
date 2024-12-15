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

    CLIState getState() const { return _currentState; }

    // Used by tests
    static std::string_view getWelcomeMessage() { return WELCOME_MESSAGE; }
    static std::string_view getLoggedOutMessage() { return LOGGED_OUT_MESSAGE; }
    static std::string_view getNoArgumentsMessage() { return NO_ARGUMENTS_MESSAGE; }

  private:
    // Request handlers
    void handleInvalidLoginRequest();
    void handleLoginRequest(const LoginRequest& request);
    void handleActionRequest(const ActionRequest& request);
    void handleGlobalCommand(const std::string_view& command, const std::vector<std::string>& args);
    void handleSpecialKey(const ActionRequest& request);
    void handleTabCompletion(const ActionRequest& request);

    // Path operations
    NodeIf* resolvePath(const Path& path) const;
    bool validatePathAccess(const NodeIf* node) const;

    // State management 
    void resetToRoot();
    void displayPrompt() const;

    TerminalIf& _terminal;
    InputParser _parser;

    std::vector<User> _users;
    std::optional<User> _currentUser;

    std::unique_ptr<Directory> _rootDirectory;
    Directory* _currentDirectory;
    PathResolver _pathResolver;

    CLIState _currentState;

    static const std::unordered_set<std::string_view> GLOBAL_COMMANDS;

    static constexpr std::string_view WELCOME_MESSAGE = "Welcome to CLI Service.";
    static constexpr std::string_view LOGGED_OUT_MESSAGE = "Logged out. Please enter <username>:<password>";
    static constexpr std::string_view EXIT_MESSAGE = "Exiting CLI Service.";
    static constexpr std::string_view NO_ARGUMENTS_MESSAGE = "Command takes no arguments.";
  };

}
