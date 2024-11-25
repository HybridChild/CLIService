#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <cstdint>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#endif

#include "io/IOStreamIf.hpp"

class StdIOStream : public IOStreamIf {
public:
  StdIOStream() {
#ifndef _WIN32
    // Save original terminal settings
    tcgetattr(STDIN_FILENO, &_origTermios);
    
    // Configure terminal for raw input
    struct termios raw = _origTermios;
    raw.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
    raw.c_cc[VMIN] = 0;   // Return immediately with what is available
    raw.c_cc[VTIME] = 0;  // No timeout
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) < 0) {
      _lastError = strerror(errno);
    }
    
    // Set stdin to non-blocking
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK) < 0) {
      _lastError = strerror(errno);
    }
#else
    // Get handle and save original console mode
    _hStdin = GetStdHandle(STD_INPUT_HANDLE);
    _hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (_hStdin == INVALID_HANDLE_VALUE || _hStdout == INVALID_HANDLE_VALUE) {
      _lastError = "Failed to get handle";
      return;
    }

    if (!GetConsoleMode(_hStdin, &_origConsoleMode)) {
      _lastError = "Failed to get console mode";
      return;
    }
    
    // Configure for raw input
    DWORD mode = _origConsoleMode & 
                 ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    if (!SetConsoleMode(_hStdin, mode)) {
      _lastError = "Failed to set console mode";
    }
#endif
  }

  ~StdIOStream() {
#ifndef _WIN32
    // Restore original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &_origTermios);
#else
    // Restore original console mode
    if (_hStdin != INVALID_HANDLE_VALUE) {
      SetConsoleMode(_hStdin, _origConsoleMode);
    }
#endif
  }

  bool putChar(char c) override {
#ifdef _WIN32
    DWORD written;
    return WriteConsoleA(_hStdout, &c, 1, &written, nullptr) && written == 1;
#else
    return write(STDOUT_FILENO, &c, 1) == 1;
#endif
  }

  bool putString(std::string_view str) override {
#ifdef _WIN32
    DWORD written;
    const DWORD length = static_cast<DWORD>(str.length());
    return WriteConsoleA(_hStdout, str.data(), length, &written, nullptr) && 
           written == length;
#else
    const ssize_t length = static_cast<ssize_t>(str.length());
    return write(STDOUT_FILENO, str.data(), str.length()) == length;
#endif
  }

  void handleBackspace() {
#ifdef _WIN32
    // Get console screen buffer info
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(_hStdout, &csbi)) {
      // If we're not at the start of the line
      if (csbi.dwCursorPosition.X > 0) {
        // Move cursor back
        csbi.dwCursorPosition.X--;
        SetConsoleCursorPosition(_hStdout, csbi.dwCursorPosition);
        // Write a space to clear the character
        putString(" ");
        // Move cursor back again
        SetConsoleCursorPosition(_hStdout, csbi.dwCursorPosition);
      }
    }
#else
    // Move cursor back, write a space, move cursor back again
    putString("\b \b");
#endif
  }

  bool getChar(char& c) override {
#ifdef _WIN32
    if (_kbhit()) {
      c = static_cast<char>(_getch());
      
      // Check for special keys (arrow keys, function keys, etc.)
      if (c == 0 || c == static_cast<char>(0xE0)) {
        _getch();  // Consume the second byte
        return false;  // Ignore special keys
      }

      if (c == '\r') {  // When Enter is pressed
        putChar('\r');
        putChar('\n');
      } else if (c == '\b') {  // Backspace
        handleBackspace();
      } else {
        putChar(c);  // Echo the character
      }
      return true;
    }
#else
    int result = read(STDIN_FILENO, &c, 1);
    if (result == 1) {
      // Check for escape sequences (arrow keys, etc.)
      if (c == '\x1B') {  // ESC character
        char seq[2];
        // Try to read the escape sequence
        if (read(STDIN_FILENO, seq, 1) == 1) {
          if (seq[0] == '[') {
            if (read(STDIN_FILENO, seq + 1, 1) == 1) {
              // Ignore arrow keys and other escape sequences
              return false;
            }
          }
        }
        return false;
      }

      if (c == '\r') {  // When Enter is pressed
        putChar('\r');
        putChar('\n');
      } else if (c == '\b' || c == 127) {  // Backspace or Delete
        handleBackspace();
      } else {
        putChar(c);
      }
      return true;
    }
    if (result < 0 && errno != EAGAIN) {
      _lastError = strerror(errno);
    }
#endif
    return false;
  }

  bool getCharTimeout(char& c, uint32_t timeout_ms) override {
    auto start = std::chrono::steady_clock::now();
    
    do {
      if (getChar(c)) {
        return true;
      }
      
      // Small sleep to prevent CPU hogging
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      
      auto now = std::chrono::steady_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
      
      if (elapsed.count() >= timeout_ms) {
        return false;
      }
    } while (true);
  }

  bool available() const override {
#ifdef _WIN32
    return _kbhit() != 0;
#else
    int bytesWaiting;
    if (ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting) < 0) {
      return false;
    }
    return bytesWaiting > 0;
#endif
  }

  void flush() override {
#ifdef _WIN32
    while (_kbhit()) {
      _getch();
    }
#else
    tcflush(STDIN_FILENO, TCIFLUSH);
#endif
  }

  bool isOpen() const override {
#ifdef _WIN32
    return _hStdin != INVALID_HANDLE_VALUE && _hStdout != INVALID_HANDLE_VALUE;
#else
    return true;  // stdin is always open
#endif
  }

  bool hasError() const override {
    return !_lastError.empty();
  }

  const char* getLastError() const override {
    return _lastError.c_str();
  }

  void clearError() override {
    _lastError.clear();
  }

private:
  std::string _lastError;

#ifdef _WIN32
  HANDLE _hStdin = INVALID_HANDLE_VALUE;
  HANDLE _hStdout = INVALID_HANDLE_VALUE;
  DWORD _origConsoleMode = 0;
#else
  struct termios _origTermios;
#endif
};
