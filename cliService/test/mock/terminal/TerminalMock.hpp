#pragma once

#include "cliService/terminal/TerminalIf.hpp"
#include <queue>
#include <string>

namespace cliService
{

  class TerminalMock : public TerminalIf
  {
  public:
    TerminalMock() = default;

    bool putChar(char c) override;
    bool getChar(char& c) override;
    bool getCharTimeout(char& c, uint32_t timeout_ms) override;
    bool available() const override;
    void flush() override;
    bool isOpen() const override;
    bool hasError() const override;
    const char* getLastError() const override;
    void clearError() override;

    // Helper methods for testing
    void queueInput(const std::string& input);
    std::string getOutput() const;
    void clearOutput();

  private:
    std::queue<char> _inputQueue;
    std::string _output;
    bool _isOpen{true};
    std::string _lastError;
  };

}
