#pragma once
#include "cliService/cli/CLIServiceConfiguration.hpp"
#include "cliService/cli/CLIState.hpp"
#include "cliService/cli/CommandHistory.hpp"
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

    Response handleRequest(const RequestBase& request);
    CLIState getState() const { return _currentState; }

  protected:

    enum class NodeDisplayMode {
      Tree,       // Show full tree with hierarchical indentation
      FlatList    // Show only immediate children with fixed indentation
    };

    NodeIf* resolvePath(const Path& path) const;
    bool validatePathAccess(const NodeIf* node) const;

    void resetToRoot();

    std::string getPromptString() const;

    std::string formatNodeInfo(const NodeIf& node, const std::string& indent, bool showCmdDescription) const;
    std::string getNodeListDisplay(NodeDisplayMode mode, bool showCmdDescription) const;

  private:
    Directory* getRootPtr() const;

    // Request handlers
    Response handleRequest(const InvalidLoginRequest& request);
    Response handleRequest(const LoginRequest& request);
    Response handleRequest(const CommandRequest& request);
    Response handleRequest(const TabCompletionRequest& request);
    Response handleRequest(const HistoryNavigationRequest& request);

    // Global command handlers
    Response handleGlobalCommand(const std::string_view& command, const std::vector<std::string>& args);
    Response handleGlobalLogout(const std::vector<std::string>& args);
    Response handleGlobalExit(const std::vector<std::string>& args);
    Response handleGlobalTree(const std::vector<std::string>& args);
    Response handleGlobalHelp(const std::vector<std::string>& args);
    Response handleGlobalQuestionMark(const std::vector<std::string>& args);
    Response handleGlobalClear(const std::vector<std::string>& args);

    void handleOutput(const Response& response);
    std::vector<std::string> splitString(const std::string& str, const std::string& delimiter);

    CharIOStreamIf& _ioStream;
    InputParser _inputParser;

    CommandHistory _commandHistory;
    std::string _savedBuffer;  // For saving current input during history navigation

    std::vector<User> _users;
    std::optional<User> _currentUser;

    std::variant<Directory*, std::unique_ptr<Directory>> _rootDirectory;
    Directory* _currentDirectory;
    PathResolver _pathResolver;

    CLIState _currentState;
    const CLIMessages _messages;

    using GlobalCommandHandler = Response (CLIService::*)(const std::vector<std::string>&);
    static const std::unordered_map<std::string_view, GlobalCommandHandler> GLOBAL_COMMAND_HANDLERS;
  };

}
