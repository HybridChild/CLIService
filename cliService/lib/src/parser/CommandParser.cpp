#include "cliService/parser/CommandParser.hpp"
#include <cassert>

namespace cliService
{

  CommandParser::CommandParser(TerminalIf& terminal, const CLIState& currentState)
    : _terminal(terminal)
    , _currentState(currentState)
    , _inEscapeSequence(false)
    , _escapeIndex(0)
  {
  }

  std::optional<std::unique_ptr<RequestBase>> CommandParser::service()
  {
    while (_terminal.available())
    {
      if (processNextChar())
      {
        return createRequest();
      }
    }
    
    return std::nullopt;
  }

  bool CommandParser::processNextChar()
  {
    char c;

    if (!_terminal.getChar(c))
    {
      return false;
    }

    if (_inEscapeSequence)
    {
      _escapeBuffer[_escapeIndex++] = c;
      if (_escapeIndex == 2)
      {
        return handleEscapeSequence();
      }
      return false;
    }

    if (c == ENTER)
    {
      _terminal.putChar('\n');

      if (!_buffer.empty())
      {
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

    if (std::iscntrl(c))
    {
      return handleControlCharacter(c);
    }

    handleRegularCharacter(c);
    return false;
  }

  bool CommandParser::handleControlCharacter(char c)
  {
    switch (c)
    {
      case BACKSPACE:
        if (!_buffer.empty())
        {
          _buffer.pop_back();
          _terminal.putString("\b \b");
        }
        break;

      case TAB:
        if (_currentState == CLIState::LoggedIn)
        {
          _buffer = "key:tab";
          return true;
        }
        break;
    }
    
    return false;
  }

  bool CommandParser::handleEscapeSequence()
  {
    _inEscapeSequence = false;

    if (_escapeBuffer[0] == '[')
    {
      switch (_escapeBuffer[1])
      {
        case 'A': // Up arrow
          if (_currentState == CLIState::LoggedIn)
          {
            _buffer = "key:up";
            return true;
          }
          break;
        
        case 'B': // Down arrow
          if (_currentState == CLIState::LoggedIn)
          {
            _buffer = "key:down";
            return true;
          }
          break;

        case 'C': // Right arrow
          if (_currentState == CLIState::LoggedIn)
          {
            _buffer = "key:right";
            return true;
          }
          break;

        case 'D': // Left arrow
          if (_currentState == CLIState::LoggedIn)
          {
            _buffer = "key:left";
            return true;
          }
          break;
      }
    }
    return false;
  }

  void CommandParser::handleRegularCharacter(char c)
  {
    _buffer += c;
    echoCharacter(c);
  }

  void CommandParser::echoCharacter(char c)
  {
    if (_currentState == CLIState::LoggedOut)
    {
      size_t colonPos = _buffer.find(':');
      // If we've found a colon and this character is after it, mask it
      if (colonPos != std::string::npos && _buffer.length() > colonPos + 1)
      {
        _terminal.putChar('*');
      }
      else
      {
        _terminal.putChar(c);
      }
    }
    else
    {
      _terminal.putChar(c);
    }
  }

  std::unique_ptr<RequestBase> CommandParser::createRequest()
  {
    switch (_currentState)
    {
      case CLIState::LoggedOut:
      {
        assert(!_buffer.empty() && "Empty input received in LoggedOut state");
        auto request = std::make_unique<LoginRequest>(_buffer);
        _buffer.clear();
        return request;
      }

      case CLIState::LoggedIn:
      {
        auto request = std::make_unique<ActionRequest>(_buffer);
        _buffer.clear();
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
