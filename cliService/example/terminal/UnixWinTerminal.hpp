#pragma once
#include "cliService/cli/TerminalIf.hpp"
#include <atomic>
#include <string>

namespace cliService
{

  class UnixWinTerminal : public TerminalIf
  {
  public:
    UnixWinTerminal();
    ~UnixWinTerminal() override;

    bool putChar(char c) override;
    bool getChar(char& c) override;
    bool getCharTimeout(char& c, uint32_t timeout_ms) override;
    bool available() const override;
    void flush() override;
    bool isOpen() const override;
    bool hasError() const override;
    const char* getLastError() const override;
    void clearError() override;

  private:
    void setupTerminal();
    void restoreTerminal();

    std::atomic<bool> _isOpen;
    std::string _lastError;

  #ifdef _WIN32
    void* _hStdin;  // HANDLE type
    void* _hStdout; // HANDLE type
    void* _oldInMode;  // DWORD type
    void* _oldOutMode; // DWORD type
  #else
    void* _oldTermios;  // struct termios*
  #endif
  };

}
