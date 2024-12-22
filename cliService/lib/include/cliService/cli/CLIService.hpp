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
  
    enum class NodeDisplayMode {
      Tree,       // Show full tree with hierarchical indentation
      FlatList    // Show only immediate children with fixed indentation
    };

    NodeIf* resolvePath(const Path& path) const;
    bool validatePathAccess(const NodeIf* node) const;

    void resetToRoot();
    void displayMessage(const std::string_view& message) const;
    void displayPrompt() const;
    void displayNewLine(uint32_t number = 1) const;
    void displayNoArgumentsError() const;

    std::string formatNodeInfo(const NodeIf& node, const std::string& indent, bool showCmdDescription) const;
    void displayNodeList(NodeDisplayMode mode, bool showCmdDescription) const;

  private:
    Directory* getRootPtr() const;

    // Request handlers
    void handleInvalidLoginRequest();
    void handleLoginRequest(const LoginRequest& request);
    void handleActionRequest(const ActionRequest& request);
    void handleGlobalCommand(const std::string_view& command, const std::vector<std::string>& args);
    void handleSpecialKey(const ActionRequest& request);
    void handleTabCompletion(const ActionRequest& request);

    // Global command handlers
    void handleGlobalLogout(const std::vector<std::string>& args);
    void handleGlobalExit(const std::vector<std::string>& args);
    void handleGlobalTree(const std::vector<std::string>& args);
    void handleGlobalHelp(const std::vector<std::string>& args);
    void handleGlobalQuestionMark(const std::vector<std::string>& args);
    void handleGlobalClear(const std::vector<std::string>& args);

    CharIOStreamIf& _ioStream;
    InputParser _inputParser;

    std::vector<User> _users;
    std::optional<User> _currentUser;

    std::variant<Directory*, std::unique_ptr<Directory>> _rootDirectory;
    Directory* _currentDirectory;
    PathResolver _pathResolver;

    CLIState _currentState;

    using GlobalCommandHandler = void (CLIService::*)(const std::vector<std::string>&);
    static const std::unordered_map<std::string_view, GlobalCommandHandler> GLOBAL_COMMAND_HANDLERS;
  };

}
