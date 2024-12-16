#include "mock/io/CharIOStreamMock.hpp"

namespace cliService
{

  bool CharIOStreamMock::putChar(char c)
  {
    _output += c;
    return true;
  }


  bool CharIOStreamMock::getChar(char& c)
  {
    if (_inputQueue.empty()) {
      return false;
    }

    c = _inputQueue.front();
    _inputQueue.pop();
    return true;
  }


  bool CharIOStreamMock::getCharTimeout(char& c, uint32_t timeout_ms)
  {
    return getChar(c);  // For mock, ignore timeout
  }


  bool CharIOStreamMock::available() const {
    return !_inputQueue.empty();
  }


  void CharIOStreamMock::flush()
  {
    // Clear input queue
    std::queue<char> empty;
    std::swap(_inputQueue, empty);
  }


  bool CharIOStreamMock::isOpen() const {
    return _isOpen;
  }


  bool CharIOStreamMock::hasError() const {
    return !_lastError.empty();
  }


  const char* CharIOStreamMock::getLastError() const {
    return _lastError.c_str();
  }


  void CharIOStreamMock::clearError() {
    _lastError.clear();
  }


  void CharIOStreamMock::queueInput(const std::string& input)
  {
    for (char c : input)
    {
      _inputQueue.push(c);
    }
  }


  std::string CharIOStreamMock::getOutput() const {
    return _output;
  }


  void CharIOStreamMock::clearOutput() {
    _output.clear();
  }

}
