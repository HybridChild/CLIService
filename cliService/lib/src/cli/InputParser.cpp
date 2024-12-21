#include "cliService/cli/InputParser.hpp"
#include <cassert>

namespace cliService
{

  InputParser::InputParser(CharIOStreamIf& ioStream, const CLIState& currentState, size_t historySize)
    : _ioStream(ioStream)
    , _history(historySize)
    , _currentState(currentState)
    , _inEscapeSequence(false)
    , _escapeBuffer(MAX_ESCAPE_LENGTH)
    , _escapeIndex(0)
  {}


  std::optional<std::unique_ptr<RequestBase>> InputParser::parseNextRequest()
  {
    while (_ioStream.available())
    {
      if (processNextChar()) {
        return createRequest();
      }
    }
    
    return std::nullopt;
  }


  bool InputParser::processNextChar()
  {
    char c;

    if (!_ioStream.getChar(c)) {
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

    if (c == ENTER_MAC || c == ENTER_WIN)
    {
      if (!_buffer.empty())
      {
        _ioStream.putString("\r\n");  // Echo newline
        _lastTrigger = ActionRequest::Trigger::Enter;
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
      case BACKSPACE:
        if (!_buffer.empty())
        {
          _buffer.pop_back();
          _ioStream.putString("\b \b");
        }
        break;

      case TAB:
        if (_currentState == CLIState::LoggedIn)
        {
          _lastTrigger = ActionRequest::Trigger::Tab;
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
          // Save current buffer first time we press up
          if (_history.getCurrentIndex() == _history.size()) {
            _savedBuffer = _buffer;
          }
          
          // Clear current line
          while (!_buffer.empty()) {
            _ioStream.putString("\b \b");
            _buffer.pop_back();
          }

          // Show previous command
          std::string prevCmd = _history.getPreviousCommand();
          _buffer = prevCmd;
          _ioStream.putString(prevCmd);
          
          _lastTrigger = ActionRequest::Trigger::ArrowUp;
          return true;
        }
        break;
      
      case 'B': // Down arrow
        if (_currentState == CLIState::LoggedIn)
        {
          // Clear current line
          while (!_buffer.empty())
          {
            _ioStream.putString("\b \b");
            _buffer.pop_back();
          }

          // Show next command or restore saved buffer
          std::string nextCmd = _history.getNextCommand();

          if (nextCmd.empty() && !_savedBuffer.empty())
          {
            nextCmd = _savedBuffer;
            _savedBuffer.clear();
          }
          
          _buffer = nextCmd;
          _ioStream.putString(nextCmd);
          
          _lastTrigger = ActionRequest::Trigger::ArrowDown;
          return true;
        }
        break;

      case 'C': // Right arrow
        if (_currentState == CLIState::LoggedIn)
        {
          _lastTrigger = ActionRequest::Trigger::ArrowRight;
          return true;
        }
        break;

      case 'D': // Left arrow
        if (_currentState == CLIState::LoggedIn)
        {
          _lastTrigger = ActionRequest::Trigger::ArrowLeft;
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
    else
    {
      _ioStream.putChar(c);
    }
  }


  std::unique_ptr<RequestBase> InputParser::createRequest()
  {
    switch (_currentState)
    {
      case CLIState::LoggedOut:
      {
        if (_buffer.empty()) {
          return nullptr;
        }

        auto loginRequest = LoginRequest::create(_buffer);
        _buffer.clear();

        if (!loginRequest) {
          return std::make_unique<InvalidLoginRequest>();
        }

        return std::make_unique<LoginRequest>(std::move(*loginRequest));
      }

      case CLIState::LoggedIn:
      {
        auto request = std::make_unique<ActionRequest>(_buffer, _lastTrigger);
        
        if (_lastTrigger == ActionRequest::Trigger::Enter && !_buffer.empty())
        {
          _history.addCommand(_buffer);
          _history.resetNavigation();
          _savedBuffer.clear();
          _buffer.clear();
        }
        
        return request;
      }

      case CLIState::Inactive:
      default:
      {
        assert(false && "Invalid CLI state");
        break;
      }
    }

    assert(false && "Invalid CLI state");
    return nullptr;
  }

}
