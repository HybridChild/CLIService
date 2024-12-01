#include "cliService/parser/InputHandler.hpp"
#include <cassert>

namespace cliService
{

  InputHandler::InputHandler(TerminalIf& terminal, const CLIState& currentState)
    : _terminal(terminal)
    , _currentState(currentState)
    , _inEscapeSequence(false)
    , _escapeIndex(0)
    , _lastTrigger(ActionRequest::Trigger::Enter)
  {
  }

  std::optional<std::unique_ptr<RequestBase>> InputHandler::service()
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

  bool InputHandler::processNextChar()
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

    if (std::iscntrl(c))
    {
      return handleControlCharacter(c);
    }

    handleRegularCharacter(c);
    return false;
  }

  bool InputHandler::handleControlCharacter(char c)
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
          _lastTrigger = ActionRequest::Trigger::Tab;
          return true;
        }
        break;
    }
    
    return false;
  }

  bool InputHandler::handleEscapeSequence()
  {
    _inEscapeSequence = false;

    if (_escapeBuffer[0] == '[')
    {
      switch (_escapeBuffer[1])
      {
        case 'A': // Up arrow
          if (_currentState == CLIState::LoggedIn)
          {
            _lastTrigger = ActionRequest::Trigger::ArrowUp;
            return true;
          }
          break;
        
        case 'B': // Down arrow
          if (_currentState == CLIState::LoggedIn)
          {
            _lastTrigger = ActionRequest::Trigger::ArrowDown;
            return true;
          }
          break;

        case 'C': // Right arrow
          // Handle right arrow if needed
          break;

        case 'D': // Left arrow
          // Handle left arrow if needed
          break;
      }
    }
    return false;
  }

  void InputHandler::handleRegularCharacter(char c)
  {
    _buffer += c;
    echoCharacter(c);
  }

  void InputHandler::echoCharacter(char c)
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

  std::unique_ptr<RequestBase> InputHandler::createRequest()
  {
    assert(!_buffer.empty() && "Empty input received");

    switch (_currentState)
    {
      case CLIState::LoggedOut:
      {
        auto request = std::make_unique<LoginRequest>(_buffer);
        _buffer.clear();
        return request;
      }

      case CLIState::LoggedIn:
      {
        auto request = std::make_unique<ActionRequest>(_buffer, _lastTrigger);
        _buffer.clear();
        return request;
      }
    }

    assert(false && "Invalid CLI state");
    return nullptr;
  }

}
