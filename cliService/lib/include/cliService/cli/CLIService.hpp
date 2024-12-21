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

    const CLIMessages _messages;

  protected:
    // Path operations
    NodeIf* resolvePath(const Path& path) const;
    bool validatePathAccess(const NodeIf* node) const;

    // State management 
    void resetToRoot();
    void displayPrompt() const;
    void displayNewLine(uint32_t number = 1) const;

  private:
    // Request handlers
    void handleInvalidLoginRequest();
    void handleLoginRequest(const LoginRequest& request);
    void handleActionRequest(const ActionRequest& request);
    void handleGlobalCommand(const std::string_view& command, const std::vector<std::string>& args);
    void handleSpecialKey(const ActionRequest& request);
    void handleTabCompletion(const ActionRequest& request);

    CharIOStreamIf& _ioStream;
    InputParser _parser;

    std::vector<User> _users;
    std::optional<User> _currentUser;

    std::unique_ptr<Directory> _rootDirectory;
    Directory* _currentDirectory;
    PathResolver _pathResolver;

    CLIState _currentState;

    // Global command handlers
    void handleGlobalLogout(const std::vector<std::string>& args);
    void handleGlobalExit(const std::vector<std::string>& args);
    void handleGlobalTree(const std::vector<std::string>& args);
    void handleGlobalHelp(const std::vector<std::string>& args);
    void handleGlobalQuestionMark(const std::vector<std::string>& args);
    void handleGlobalClear(const std::vector<std::string>& args);

    using GlobalCommandHandler = void (CLIService::*)(const std::vector<std::string>&);
    static const std::unordered_map<std::string_view, GlobalCommandHandler> GLOBAL_COMMAND_HANDLERS;
  };

}
