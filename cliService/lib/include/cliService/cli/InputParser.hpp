#pragma once
#include "cliService/cli/CharIOStreamIf.hpp"
#include "cliService/cli/ActionRequest.hpp"
#include "cliService/cli/LoginRequest.hpp"
#include "cliService/cli/CLIState.hpp"
#include "cliService/cli/CommandHistory.hpp"
#include <memory>
#include <string>
#include <optional>

namespace cliService
{

  class InputParser
  {
  public:
    static constexpr size_t MAX_ESCAPE_LENGTH = 16;
    
    static constexpr char BACKSPACE_DEL = 0x7F;  // ASCII DEL
    static constexpr char BACKSPACE_BS = 0x08;   // ASCII backspace
    static constexpr char ENTER_LF = 0x0A;
    static constexpr char ENTER_CR = 0x0D;
    static constexpr char TAB = 0x09;
    static constexpr char ESC = 0x1B;

    InputParser(CharIOStreamIf& ioStream, const CLIState& currentState, uint32_t inputTimeout_ms, size_t historySize);

    std::optional<std::unique_ptr<RequestBase>> parseNextRequest();

    std::string getBuffer() const { return _buffer; }
    void appendToBuffer(const std::string& str) { _buffer += str; }

  private:
    bool processNextChar();
    bool handleControlCharacter(char c);
    bool handleEscapeSequence();
    void handleRegularCharacter(char c);
    void echoCharacter(char c);
    std::unique_ptr<RequestBase> createRequest();

    CharIOStreamIf& _ioStream;
    std::string _buffer;
    uint32_t _inputTimeout_ms;
    CommandHistory _history;
    std::string _savedBuffer;  // Saves current input when navigating history

    ActionRequest::Trigger _lastTrigger;
    const CLIState& _currentState;

    bool _inEscapeSequence;
    std::vector<char> _escapeBuffer;
    size_t _escapeIndex;
  };

}
