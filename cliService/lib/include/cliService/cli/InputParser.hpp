#pragma once
#include "cliService/cli/CharIOStreamIf.hpp"
#include "cliService/cli/CommandRequest.hpp"
#include "cliService/cli/LoginRequest.hpp"
#include "cliService/cli/TabCompletionRequest.hpp"
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

    enum class ActionTrigger
    {
      Enter,
      Tab,
      ArrowUp,
      ArrowDown,
    };

    struct ParsedPathAndArgs {
      Path path;
      std::vector<std::string> args;
    };

    InputParser(CharIOStreamIf& ioStream, const CLIState& currentState, uint32_t inputTimeout_ms, size_t historySize);

    std::optional<std::unique_ptr<RequestBase>> getNextRequest();

    std::string getBuffer() const { return _buffer; }
    void appendToBuffer(const std::string& str) { _buffer += str; }

    static std::optional<LoginRequest> parseToLoginRequest(const std::string& input)
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

    static std::unique_ptr<CommandRequest> parseToCommandRequest(std::string_view input)
    {
      ParsedPathAndArgs parsedPath = parseToPathAndArgs(input);
      return std::make_unique<CommandRequest>(parsedPath.path, std::move(parsedPath.args));
    }

    static std::unique_ptr<TabCompletionRequest> parseToTabCompletionRequest(std::string_view input)
    {
      ParsedPathAndArgs parsedPath = parseToPathAndArgs(input);
      return std::make_unique<TabCompletionRequest>(parsedPath.path);
    }

  private:
    bool processNextChar();
    static ParsedPathAndArgs parseToPathAndArgs(std::string_view input);
    std::optional<std::unique_ptr<RequestBase>> createRequest();

    bool handleControlCharacter(char c);
    bool handleEscapeSequence();
    void handleRegularCharacter(char c);
    void echoCharacter(char c);


    CharIOStreamIf& _ioStream;
    std::string _buffer;
    uint32_t _inputTimeout_ms;
    CommandHistory _history;
    std::string _savedBuffer;  // Saves current input when navigating history

    ActionTrigger _lastTrigger;
    const CLIState& _currentState;

    bool _inEscapeSequence;
    std::vector<char> _escapeBuffer;
    size_t _escapeIndex;
  };

}
