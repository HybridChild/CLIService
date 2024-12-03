#pragma once

#include "cliService/terminal/TerminalIf.hpp"
#include "cliService/requests/ActionRequest.hpp"
#include "cliService/requests/LoginRequest.hpp"
#include "cliService/cli/CLIState.hpp"
#include <memory>
#include <string>
#include <optional>

namespace cliService
{

  class InputHandler 
  {
  public:
    InputHandler(TerminalIf& terminal, const CLIState& currentState);

    std::optional<std::unique_ptr<RequestBase>> service();

  private:
    static constexpr char BACKSPACE = 0x7F;
    static constexpr char ENTER = 0x0D;
    static constexpr char TAB = 0x09;
    static constexpr char ESC = 0x1B;

    bool processNextChar();
    bool handleControlCharacter(char c);
    bool handleEscapeSequence();
    void handleRegularCharacter(char c);
    void echoCharacter(char c);
    std::unique_ptr<RequestBase> createRequest();

    TerminalIf& _terminal;
    std::string _buffer;
    const CLIState& _currentState;
    bool _inEscapeSequence;
    char _escapeBuffer[2];
    uint8_t _escapeIndex;
    ActionRequest::Trigger _lastTrigger;
  };

}
