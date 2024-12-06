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

  class CommandParser 
  {
  public:
    CommandParser(TerminalIf& terminal, const CLIState& currentState);

    std::optional<std::unique_ptr<RequestBase>> service();

    static constexpr char BACKSPACE = 0x7F;
    static constexpr char ENTER = 0x0A;
    static constexpr char TAB = 0x09;
    static constexpr char ESC = 0x1B;

  private:
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
  };

}
