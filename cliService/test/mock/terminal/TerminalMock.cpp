#include "mock/terminal/TerminalMock.hpp"

namespace cliService
{

  bool TerminalMock::putChar(char c)
  {
    _output += c;
    return true;
  }


  bool TerminalMock::getChar(char& c)
  {
    if (_inputQueue.empty()) {
      return false;
    }

    c = _inputQueue.front();
    _inputQueue.pop();
    return true;
  }


  bool TerminalMock::getCharTimeout(char& c, uint32_t timeout_ms)
  {
    return getChar(c);  // For mock, ignore timeout
  }


  bool TerminalMock::available() const {
    return !_inputQueue.empty();
  }


  void TerminalMock::flush()
  {
    // Clear input queue
    std::queue<char> empty;
    std::swap(_inputQueue, empty);
  }


  bool TerminalMock::isOpen() const {
    return _isOpen;
  }


  bool TerminalMock::hasError() const {
    return !_lastError.empty();
  }


  const char* TerminalMock::getLastError() const {
    return _lastError.c_str();
  }


  void TerminalMock::clearError() {
    _lastError.clear();
  }


  void TerminalMock::queueInput(const std::string& input)
  {
    for (char c : input)
    {
      _inputQueue.push(c);
    }
  }


  std::string TerminalMock::getOutput() const {
    return _output;
  }


  void TerminalMock::clearOutput() {
    _output.clear();
  }

}
