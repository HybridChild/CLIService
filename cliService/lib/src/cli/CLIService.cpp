#include "cliService/cli/CLIService.hpp"
#include "cliService/cli/CommandRequest.hpp"
#include "cliService/cli/LoginRequest.hpp"
#include "cliService/cli/TabCompletionRequest.hpp"
#include "cliService/cli/CharIOStreamIf.hpp"
#include "cliService/cli/User.hpp"
#include "cliService/tree/CommandIf.hpp"
#include "cliService/tree/Directory.hpp"
#include "cliService/tree/PathCompleter.hpp"
#include <cassert>


namespace cliService
{

  const std::unordered_map<std::string_view, CLIService::GlobalCommandHandler> CLIService::GLOBAL_COMMAND_HANDLERS = {
    {"logout" , &CLIService::handleGlobalLogout},
    {"exit"   , &CLIService::handleGlobalExit},
    {"tree"   , &CLIService::handleGlobalTree},
    {"help"   , &CLIService::handleGlobalHelp},
    {"?"      , &CLIService::handleGlobalQuestionMark},
    {"clear"  , &CLIService::handleGlobalClear}
  };


  CLIService::CLIService(CLIServiceConfiguration config)
    : _ioStream(config._ioStream)
    , _inputParser(_ioStream, _currentState, config._inputTimeout_ms)
    , _commandHistory(config._historySize)
    , _users(std::move(config._users))
    , _currentUser(std::nullopt)
    , _rootDirectory(std::move(config._rootDirectory))
    , _currentDirectory(getRootPtr())
    , _pathResolver(*getRootPtr())
    , _currentState(CLIState::Inactive)
    , _messages(std::move(config._messages))
  {
    assert(!_users.empty() && "User list cannot be empty");
    assert(getRootPtr() != nullptr && "Root directory cannot be null");
    assert(_currentDirectory != nullptr && "Current directory must be set");
  }


  void CLIService::activate()
  {
    assert(_currentState == CLIState::Inactive && "Service must be inactive to activate");

    _currentState = CLIState::LoggedOut;
    _ioStream.putString(_messages.getNewLine());
    _ioStream.putString(std::string(_messages.getIndentation()) + std::string(_messages.getWelcomeMessage()));
    _ioStream.putString(_messages.getNewLine(2));
    _ioStream.putString(getPromptString());
  }


  void CLIService::service()
  {
    if (_currentState == CLIState::Inactive) { return; }

    // Get next request from parser
    auto requestPtr = _inputParser.getNextRequest();
    if (!requestPtr || !*requestPtr) { return; }

    // Get response from appropriate handler
    Response response = handleRequest(**requestPtr);

    // Handle output
    handleOutput(response);
  }


  Response CLIService::handleRequest(const RequestBase& request)
  {
    if (const auto* commandRequest = dynamic_cast<const CommandRequest*>(&request)) {
      return handleRequest(*commandRequest);
    }

    if (const auto* tabRequest = dynamic_cast<const TabCompletionRequest*>(&request)) {
      return handleRequest(*tabRequest);
    }

    if (const auto* loginRequest = dynamic_cast<const LoginRequest*>(&request)) {
      return handleRequest(*loginRequest);
    }

    if (const auto* invalidLoginRequest = dynamic_cast<const InvalidLoginRequest*>(&request)) {
      return handleRequest(*invalidLoginRequest);
    }

    if (const auto* historyRequest = dynamic_cast<const HistoryNavigationRequest*>(&request)) {
      return handleRequest(*historyRequest);
    }

    return Response(static_cast<std::string>("Unknown request type"), ResponseStatus::Error);
  }


  NodeIf* CLIService::resolvePath(const Path& path) const {
    return _pathResolver.resolve(path, *_currentDirectory);
  }


  bool CLIService::validatePathAccess(const NodeIf* node) const
  {
    assert(_currentUser && "No user logged in");

    if (!node) { return false; }

    // Check access levels up the tree
    const NodeIf* current = node;

    while (current)
    {
      if (current->getAccessLevel() > _currentUser->getAccessLevel()) {
        return false;
      }

      current = current->getParent();
    }

    return true;
  }


  void CLIService::resetToRoot() {
    _currentDirectory = getRootPtr();
  }


  std::string CLIService::getPromptString() const
  {
    std::string prompt = "";

    if (_currentState == CLIState::LoggedIn && _currentUser)
    {
      prompt += _currentUser->getUsername() + "@";
      prompt += _pathResolver.getAbsolutePath(*_currentDirectory).toString();
    }

    prompt += "> ";
    return prompt;
  }


  std::string CLIService::formatNodeInfo(const NodeIf& node, const std::string& indent, bool showCmdDescription) const
  {
    std::string nodeStr = indent + node.getName();

    if (node.isDirectory()) {
      nodeStr += "/";
    }
    else if (auto* cmd = dynamic_cast<const CommandIf*>(&node))
    {
      if (showCmdDescription && !cmd->getDescription().empty()) {
        nodeStr += " - " + cmd->getDescription();
      }
    }

    return nodeStr;
  }


  std::string CLIService::getNodeListDisplay(NodeDisplayMode mode, bool showCmdDescription) const
  {
    std::string nodeList = "";

    _currentDirectory->traverse([&](const NodeIf& node, size_t depth) {
      if (mode == NodeDisplayMode::FlatList && depth != 1) {
        return; // Skip nodes not at depth 1 for flat list
      }

      if (node.getAccessLevel() <= _currentUser->getAccessLevel()) {
        std::string indent = "";

        if (mode == NodeDisplayMode::Tree && depth > 0) {
          indent = std::string(depth * 2, ' ');
        }

        nodeList += formatNodeInfo(node, indent, showCmdDescription);
        nodeList += _messages.getNewLine();
      }
    });

    return nodeList;
  }


  Directory* CLIService::getRootPtr() const
  {
    if (auto staticPtr = std::get_if<Directory*>(&_rootDirectory)) {
      return *staticPtr;
    }
    
    return std::get<std::unique_ptr<Directory>>(_rootDirectory).get();
  }


  Response CLIService::handleRequest(const InvalidLoginRequest& request)
  {
    (void)request;
    return Response::error(_messages.getInvalidLoginMessage());
  }


  Response CLIService::handleRequest(const LoginRequest& request)
  {
    const auto& username = request.getUsername();
    const auto& password = request.getPassword();

    auto userIt = std::find_if(_users.begin(), _users.end(),
      [&](const User& user) {
        return user.getUsername() == username && user.getPassword() == password;
      });

    Response response = Response::success();

    if (userIt != _users.end())
    {
      _currentUser = *userIt;
      _currentState = CLIState::LoggedIn;
      response.appendToMessage(_messages.getLoggedInMessage());
    }
    else {
      response.appendToMessage(_messages.getInvalidLoginMessage());
      response.setStatus(ResponseStatus::Error);
    }

    return response;
  }


  Response CLIService::handleRequest(const CommandRequest& request) 
  {
    assert(_currentUser && "No user logged in");

    const auto& path = request.getPath();
    
    if (!path.isEmpty())
    {
      const auto& command = path.elements().front();

      if (GLOBAL_COMMAND_HANDLERS.find(command) != GLOBAL_COMMAND_HANDLERS.end()) {
        return handleGlobalCommand(command, request.getArgs());
      }
    }

    Response response = Response::success();
    NodeIf* node = resolvePath(path);

    if (!node)
    {
      response.appendToMessage(_messages.getInvalidPathMessage());
      response.setStatus(ResponseStatus::InvalidPath);
      return response;
    }

    if (!validatePathAccess(node))
    {
      response.appendToMessage(_messages.getAccessDeniedMessage());
      response.setStatus(ResponseStatus::AccessDenied);
      return response;
    }

    // Handle directory navigation or command execution
    if (node->isDirectory())
    {
      _currentDirectory = static_cast<Directory*>(node);
      response.setPrefixNewLine(false);
      response.setPostfixNewLine(false);
    }
    else
    {
      auto* cmd = static_cast<CommandIf*>(node);
      response = cmd->execute(request.getArgs());
    }

    if (!request.getPath().isEmpty()) {
      _commandHistory.addCommand(request.getOriginalInput());
    }

    return response;
  }


  Response CLIService::handleGlobalCommand(const std::string_view& command, const std::vector<std::string>& args)
  {
    auto it = GLOBAL_COMMAND_HANDLERS.find(command);

    if (it != GLOBAL_COMMAND_HANDLERS.end()) {
      return (this->*(it->second))(args);
    }

    return Response("Unknown command: " + std::string(command), ResponseStatus::Error);
  }


  Response CLIService::handleRequest(const TabCompletionRequest& request)
  {
    Response response = Response::success();
    response.setShowPrompt(false);
    response.setIndentMessage(false);
    response.setInlineMessage(true);
    response.setPrefixNewLine(false);
    response.setPostfixNewLine(false);

    auto currentInput = _inputParser.getBuffer();

    // Skip directory handling for paths with parent references
    if (currentInput.find("..") == std::string::npos)
    {
      auto node = resolvePath(request.getPath());

      if (node && node->isDirectory() && !currentInput.empty() && currentInput.back() != '/')
      {
        _inputParser.appendToBuffer("/", false);
        response.appendToMessage(std::string("/"));
        return response;
      }
    }

    PathCompleter::CompletionResult result{};

    if (request.getPath().isAbsolute()) {
      result = PathCompleter::complete(*getRootPtr(), currentInput, _currentUser->getAccessLevel());
    }
    else {
      result = PathCompleter::complete(*_currentDirectory, currentInput, _currentUser->getAccessLevel());
    }

    if (result.allOptions.size() > 1)
    {
      // Just display newline (we don't need to show original input since it's already shown)
      response.appendToMessage(_messages.getNewLine());

      // Show all options
      for (const auto& opt : result.allOptions) {
        response.appendToMessage("   " + opt);
      }

      response.appendToMessage(_messages.getNewLine());
      response.appendToMessage(getPromptString()); // Show prompt with common prefix
      response.setShowPrompt(false);
      
      if (!result.fillCharacters.empty())
      {
        _inputParser.appendToBuffer(result.fillCharacters, false);
        response.appendToMessage(currentInput + result.fillCharacters);
      }
      else {
        response.appendToMessage(currentInput);
      }
    }
    else if (!result.fillCharacters.empty())
    {
      // For single match, just append completion
      _inputParser.appendToBuffer(result.fillCharacters, false);
      response.appendToMessage(result.fillCharacters);
      
      if (result.isDirectory)
      {
        _inputParser.appendToBuffer("/", false);
        response.appendToMessage(std::string("/"));
      }
    }

    return response;
  }


  Response CLIService::handleRequest(const HistoryNavigationRequest& request)
  {
    // First time pressing up, save current buffer
    if (request.getDirection() == HistoryNavigationRequest::Direction::Previous &&
        _commandHistory.getCurrentIndex() == _commandHistory.size())
    {
      _savedBuffer = request.getCurrentBuffer();
    }

    std::string historyCommand;

    if (request.getDirection() == HistoryNavigationRequest::Direction::Previous) {
      historyCommand = _commandHistory.getPreviousCommand();
    }
    else
    {
      historyCommand = _commandHistory.getNextCommand();

      if (historyCommand.empty() && !_savedBuffer.empty())
      {
        historyCommand = _savedBuffer;
        _savedBuffer.clear();
      }
    }

    _inputParser.replaceBuffer(historyCommand);

    Response response = Response::success();
    response.setShowPrompt(false);
    response.setIndentMessage(false);
    response.setPrefixNewLine(false);
    response.setPostfixNewLine(false);
    response.setInlineMessage(true);
    return response;
  }


  Response CLIService::handleGlobalHelp(const std::vector<std::string>& args)
  {
    Response response = Response::success();

    if (!args.empty())
    {
      response.setStatus(ResponseStatus::InvalidArguments);
      response.appendToMessage(std::string(_messages.getNoArgumentsMessage()));
    }
    else
    {
      response.appendToMessage("help   - List global commands" + std::string(_messages.getNewLine()));
      response.appendToMessage("tree   - Print directory tree" + std::string(_messages.getNewLine()));
      response.appendToMessage("?      - Detail items in current directory" + std::string(_messages.getNewLine()));
      response.appendToMessage("logout - Exit current session" + std::string(_messages.getNewLine()));
      response.appendToMessage("clear  - Clear screen" + std::string(_messages.getNewLine()));
      response.appendToMessage("exit   - Exit the CLI" + std::string(_messages.getNewLine(0)));
    }

    return response;
  }

  Response CLIService::handleGlobalTree(const std::vector<std::string>& args)
  {
    Response response = Response::success();

    if (!args.empty())
    {
      response.setStatus(ResponseStatus::InvalidArguments);
      response.appendToMessage(std::string(_messages.getNoArgumentsMessage()));
    }
    else
    {
      std::string nodeList = getNodeListDisplay(NodeDisplayMode::Tree, false);
      response.appendToMessage(nodeList);
      response.setPostfixNewLine(false);
    }

    return response;
  }


  Response CLIService::handleGlobalQuestionMark(const std::vector<std::string>& args)
  {
    Response response = Response::success();

    if (!args.empty())
    {
      response.setStatus(ResponseStatus::InvalidArguments);
      response.appendToMessage(std::string(_messages.getNoArgumentsMessage()));
    }
    else
    {
      std::string nodeList = getNodeListDisplay(NodeDisplayMode::FlatList, true);
      response.appendToMessage(nodeList);
      response.setPostfixNewLine(false);
    }

    return response;
  }


  Response CLIService::handleGlobalLogout(const std::vector<std::string>& args)
  {
    Response response = Response::success();

    if (!args.empty())
    {
      response.setStatus(ResponseStatus::InvalidArguments);
      response.appendToMessage(std::string(_messages.getNoArgumentsMessage()));
    }
    else
    {
      _currentState = CLIState::LoggedOut;
      _currentUser = std::nullopt;
      resetToRoot();
      response.appendToMessage(_messages.getLoggedOutMessage());
    }

    return response;
  }


  Response CLIService::handleGlobalClear(const std::vector<std::string>& args)
  {
    Response response = Response::success();

    if (!args.empty())
    {
      response.setStatus(ResponseStatus::InvalidArguments);
      response.appendToMessage(std::string(_messages.getNoArgumentsMessage()));
    }
    else
    {
      // Send ANSI escape sequence to clear screen and move cursor to home position
      response.appendToMessage(std::string("\033[2J"));
      response.appendToMessage(std::string("\033[H"));
      response.setPrefixNewLine(false);
      response.setPostfixNewLine(false);
    }

    return response;
  }


  Response CLIService::handleGlobalExit(const std::vector<std::string>& args)
  {
    Response response = Response::success();

    if (!args.empty())
    {
      response.setStatus(ResponseStatus::InvalidArguments);
      response.appendToMessage(std::string(_messages.getNoArgumentsMessage()));
    }
    else
    {
      _currentState = CLIState::Inactive;
      _currentUser = std::nullopt;
      resetToRoot();
      response.appendToMessage(std::string(_messages.getExitMessage()));
      response.setShowPrompt(false);
    }

    return response;
  }


  void CLIService::handleOutput(const Response& response)
  {
    if (response.prefixNewLine()) {
      _ioStream.putString(_messages.getNewLine());
    }

    auto lineList = splitString(response.getMessage(), std::string(_messages.getNewLine()));

    for (uint32_t idx = 0; idx < lineList.size(); ++idx)
    {
      if (response.indentMessage()) {
        _ioStream.putString(std::string(_messages.getIndentation()));
      }

      _ioStream.putString(lineList[idx]);

      if (idx < lineList.size() - 1 || !response.inlineMessage()) {
        _ioStream.putString(_messages.getNewLine());
      }
    }

    if (response.postfixNewLine()) {
      _ioStream.putString(_messages.getNewLine());
    }

    if (response.showPrompt()) {
      _ioStream.putString(getPromptString());
    }
  }


  std::vector<std::string> CLIService::splitString(const std::string& str, const std::string& delimiter)
  {
    std::vector<std::string> substrings;
    if (str.empty()) { return substrings; }

    assert(!delimiter.empty() && "Delimiter cannot be empty");

    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos)
    {
      substrings.push_back(str.substr(start, end - start));
      start = end + delimiter.length();
      end = str.find(delimiter, start);
    }

    substrings.push_back(str.substr(start));

    return substrings;
  }

}
