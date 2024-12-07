#pragma once
#include "cliService/cli/TerminalIf.hpp"
#include "cliService/cli/ActionRequest.hpp"
#include "cliService/cli/LoginRequest.hpp"
#include "cliService/cli/CLIState.hpp"
#include <memory>
#include <string>
#include <optional>

namespace cliService
{

  class InputParser 
  {
  public:
    InputParser(TerminalIf& terminal, const CLIState& currentState);

    std::optional<std::unique_ptr<RequestBase>> service();

    static constexpr char BACKSPACE = 0x7F;
    static constexpr char ENTER = 0x0A;
    static constexpr char TAB = 0x09;
    static constexpr char ESC = 0x1B;

    static constexpr std::string_view KEY_CODE_ARROW_UP = "key:up";
    static constexpr std::string_view KEY_CODE_ARROW_DOWN = "key:down";
    static constexpr std::string_view KEY_CODE_ARROW_LEFT = "key:left";
    static constexpr std::string_view KEY_CODE_ARROW_RIGHT = "key:right";
    static constexpr std::string_view KEY_CODE_TAB = "key:tab";

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
