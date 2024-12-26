#include "cliService/cli/InputParser.hpp"
#include <cassert>

namespace cliService
{

  InputParser::InputParser(CharIOStreamIf& ioStream, const CLIState& currentState, uint32_t inputTimeout_ms)
    : _currentState(currentState)
    , _ioStream(ioStream)
    , _inputTimeout_ms(inputTimeout_ms)
    , _inEscapeSequence(false)
    , _escapeBuffer(MAX_ESCAPE_LENGTH)
    , _escapeIndex(0)
  {}


  std::optional<std::unique_ptr<RequestBase>> InputParser::getNextRequest()
  {
    while (_ioStream.available())
    {
      if (processNextChar()) {
        return createRequest();
      }
    }

    return std::nullopt;
  }


  void InputParser::replaceBuffer(const std::string& newContent, bool display)
  {
    if (display)
    {
      clearDisplayedBuffer();
      _ioStream.putString(newContent);
    }

    _buffer = newContent;
  }


  void InputParser::appendToBuffer(const std::string& newContent, bool display)
  {
    if (display) {
      _ioStream.putString(newContent);
    }

    _buffer += newContent;
  }


  std::optional<LoginRequest> InputParser::parseToLoginRequest(const std::string& input)
  {
    size_t delimPos = input.find(':');
    
    if (delimPos == std::string::npos) {
      return std::nullopt;
    }

    std::string username = input.substr(0, delimPos);
    std::string password = input.substr(delimPos + 1);

    if (username.empty() || password.empty()) {
      return std::nullopt;
    }

    return LoginRequest(std::move(username), std::move(password));
  }


  std::unique_ptr<CommandRequest> InputParser::parseToCommandRequest(std::string_view input)
  {
    ParsedPathAndArgs parsedPath = parseToPathAndArgs(input);
    return std::make_unique<CommandRequest>(parsedPath.path, std::move(parsedPath.args), input);
  }


  std::unique_ptr<TabCompletionRequest> InputParser::parseToTabCompletionRequest(std::string_view input)
  {
    ParsedPathAndArgs parsedPath = parseToPathAndArgs(input);
    return std::make_unique<TabCompletionRequest>(parsedPath.path);
  }


  std::unique_ptr<HistoryNavigationRequest> InputParser::parseToHistoryNavigationRequest(std::string_view input, ActionTrigger trigger)
  {
    HistoryNavigationRequest::Direction direction;

    switch (trigger)
    {
    case ActionTrigger::ArrowUp:
      direction = HistoryNavigationRequest::Direction::Previous;
      break;
    case ActionTrigger::ArrowDown:
      direction = HistoryNavigationRequest::Direction::Next;
      break;
    case ActionTrigger::Enter:
    case ActionTrigger::Tab:
    default:
      assert(false);
      break;
    }

    return std::make_unique<HistoryNavigationRequest>(direction, input);
  }


  bool InputParser::processNextChar()
  {
    char c;

    if (!_ioStream.getCharTimeout(c, _inputTimeout_ms)) {
      return false;
    }

    if (_inEscapeSequence)
    {
      // Protect against buffer overflow
      if (_escapeIndex >= MAX_ESCAPE_LENGTH - 1)
      {
        _inEscapeSequence = false;
        _escapeIndex = 0;
        return false;
      }

      _escapeBuffer[_escapeIndex++] = c;

      // For arrow keys, we expect ESC [ X where X is A/B/C/D
      // But some terminals might send longer sequences, so we need to handle those
      if (_escapeIndex >= 2)
      {
        // Try to handle the sequence
        bool handled = handleEscapeSequence();

        if (handled)
        {
          _escapeIndex = 0;
          return true;
        }

        // If not a recognized 2-char sequence, keep reading until we 
        // hit a terminal character (usually an alphabetic character)
        if (_escapeIndex >= 2 && std::isalpha(c))
        {
          _inEscapeSequence = false;
          _escapeIndex = 0;
          return false;
        }
      }

      return false;
    }

    if (c == ENTER_CR || c == ENTER_LF)
    {
      if (!_buffer.empty())
      {
        _ioStream.putString("\r\n");  // Echo newline
        _trigger = ActionTrigger::Enter;
        return true;
      }

      return false;
    }

    if (c == ESC)
    {
      _inEscapeSequence = true;
      _escapeIndex = 0;
      return false;
    }

    if (std::iscntrl(c)) {
      return handleControlCharacter(c);
    }

    handleRegularCharacter(c);
    return false;
  }


  bool InputParser::handleControlCharacter(char c)
  {
    switch (c)
    {
    case BACKSPACE_BS:
    case BACKSPACE_DEL:
        if (!_buffer.empty())
        {
          _buffer.pop_back();
          _ioStream.putString("\b \b");
        }
        break;

      case TAB:
        if (_currentState == CLIState::LoggedIn)
        {
          _trigger = ActionTrigger::Tab;
          return true;
        }
        break;

      default:
        break;
    }

    return false;
  }


  bool InputParser::handleEscapeSequence()
  {
    _inEscapeSequence = false;

    // Basic safety check
    if (_escapeIndex < 2) {
        return false;
    }

    // Validate we have a proper CSI (Control Sequence Introducer)
    if (_escapeBuffer[0] != '[') {
        return false;
    }

    switch (_escapeBuffer[1])
    {
      case 'A': // Up arrow
        if (_currentState == CLIState::LoggedIn)
        {
          _trigger = ActionTrigger::ArrowUp;
          return true;
        }
        break;

      case 'B': // Down arrow
        if (_currentState == CLIState::LoggedIn)
        {
          _trigger = ActionTrigger::ArrowDown;
          return true;
        }
        break;

      default:
        break;
    }

    return false;
  }


  void InputParser::handleRegularCharacter(char c)
  {
    _buffer += c;
    echoCharacter(c);
  }


  void InputParser::echoCharacter(char c)
  {
    if (_currentState == CLIState::LoggedOut)
    {
      size_t colonPos = _buffer.find(':');

      // If we've found a colon and this character is after it, mask it
      if (colonPos != std::string::npos && _buffer.length() > colonPos + 1) {
        _ioStream.putChar('*');
      }
      else {
        _ioStream.putChar(c);
      }
    }
    else {
      _ioStream.putChar(c);
    }
  }


  void InputParser::clearDisplayedBuffer()
  {
    for (size_t i = 0; i < _buffer.length(); ++i) {
      _ioStream.putString("\b \b");
    }
  }


  std::optional<std::unique_ptr<RequestBase>> InputParser::createRequest()
  {
    switch (_currentState)
    {
      case CLIState::LoggedOut:
      {
        if (_buffer.empty()) { return std::nullopt; }

        auto loginRequest = parseToLoginRequest(_buffer);
        _buffer.clear();

        if (!loginRequest) {
          return std::make_unique<InvalidLoginRequest>();
        }

        return std::make_unique<LoginRequest>(std::move(*loginRequest));
      }

      case CLIState::LoggedIn:
      {
        if (_trigger == ActionTrigger::Enter)
        {
          if (_buffer.empty()) { return std::nullopt; }

          auto request = parseToCommandRequest(_buffer);
          _buffer.clear();
          return request;
        }

        if(_trigger == ActionTrigger::Tab)
        {
          auto request = parseToTabCompletionRequest(_buffer);
          return request;
        }

        if (_trigger == ActionTrigger::ArrowUp || _trigger == ActionTrigger::ArrowDown) {
          return parseToHistoryNavigationRequest(_buffer, _trigger);
        }
      }

      case CLIState::Inactive:
      default:
      {
        assert(false && "Invalid CLI state");
        break;
      }
    }

    return std::nullopt;
  }

  InputParser::ParsedPathAndArgs InputParser::parseToPathAndArgs(std::string_view input)
  {
    Path path;
    std::vector<std::string> args;

    // Split input into path and args
    std::string_view pathStr, argsStr;

    // Find first space that separates path from args
    size_t spacePos = input.find(' ');

    if (spacePos == std::string_view::npos)
    {
      // No args, entire input is path
      pathStr = input;
      argsStr = std::string_view();
    }
    else
    {
      // Split into path and args
      pathStr = input.substr(0, spacePos);
      
      // Skip spaces between path and args
      size_t argsStart = spacePos + 1;

      while (argsStart < input.length() && input[argsStart] == ' ') {
        argsStart++;
      }

      argsStr = input.substr(argsStart);

      // If we have arguments and path ends with slashes, trim them
      if (!argsStr.empty() && !pathStr.empty())
      {
        while (!pathStr.empty() && pathStr.back() == '/') {
          pathStr = pathStr.substr(0, pathStr.length() - 1);
        }
      }
    }

    // Create path object
    path = Path(pathStr);

    // Parse args if present
    if (!argsStr.empty())
    {
      // Fix for most vexing parse - use brace initialization
      std::istringstream argStream{std::string(argsStr)};
      std::string arg;

      while (argStream >> arg) {
        args.push_back(std::move(arg));
      }
    }

    return ParsedPathAndArgs{std::move(path), std::move(args)};
  }

}
