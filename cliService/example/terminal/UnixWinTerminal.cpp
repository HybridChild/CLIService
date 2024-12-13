#include "UnixWinTerminal.hpp"
#include <cassert>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#endif

namespace cliService
{

  UnixWinTerminal::UnixWinTerminal()
    : _isOpen(false), _lastError("")
  {
  #ifdef _WIN32
    _hStdin = GetStdHandle(STD_INPUT_HANDLE);
    _hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    _oldInMode = new DWORD;
    _oldOutMode = new DWORD;
    assert(_hStdin != INVALID_HANDLE_VALUE && "Failed to get stdin handle");
    assert(_hStdout != INVALID_HANDLE_VALUE && "Failed to get stdout handle");
  #else
    _oldTermios = new termios;
  #endif

    setupTerminal();
    _isOpen = true;
  }

  UnixWinTerminal::~UnixWinTerminal()
  {
    restoreTerminal();

  #ifdef _WIN32
    delete static_cast<DWORD*>(_oldInMode);
    delete static_cast<DWORD*>(_oldOutMode);
  #else
    delete static_cast<termios*>(_oldTermios);
  #endif
  }

  void UnixWinTerminal::setupTerminal()
  {
  #ifdef _WIN32
    // Get current console mode
    GetConsoleMode(_hStdin, static_cast<DWORD*>(_oldInMode));
    GetConsoleMode(_hStdout, static_cast<DWORD*>(_oldOutMode));

    // Set new console mode for stdin
    DWORD newMode = ENABLE_VIRTUAL_TERMINAL_INPUT |
                    ENABLE_PROCESSED_INPUT;

    if (!SetConsoleMode(_hStdin, newMode))
    {
      _lastError = "Failed to set console input mode";
      _isOpen = false;
      return;
    }

    // ... and stdout
    newMode = ENABLE_VIRTUAL_TERMINAL_PROCESSING | 
              ENABLE_PROCESSED_OUTPUT |
              ENABLE_WRAP_AT_EOL_OUTPUT;

    if (!SetConsoleMode(_hStdout, newMode))
    {
        _lastError = "Failed to set console output mode";
        _isOpen = false;
        return;
    }
  #else
    // Get current terminal attributes
    struct termios newTermios;
    assert(tcgetattr(STDIN_FILENO, static_cast<termios*>(_oldTermios)) == 0 && "Failed to get terminal attributes");
    newTermios = *static_cast<termios*>(_oldTermios);

    // Disable canonical mode and echo
    newTermios.c_lflag &= ~(ICANON | ECHO);
    newTermios.c_cc[VMIN] = 1;
    newTermios.c_cc[VTIME] = 0;

    assert(tcsetattr(STDIN_FILENO, TCSANOW, &newTermios) == 0 && "Failed to set terminal attributes");
  #endif
  }

  void UnixWinTerminal::restoreTerminal()
  {
  #ifdef _WIN32
    SetConsoleMode(_hStdin, *static_cast<DWORD*>(_oldInMode));
    SetConsoleMode(_hStdout, *static_cast<DWORD*>(_oldOutMode));
  #else
    tcsetattr(STDIN_FILENO, TCSANOW, static_cast<termios*>(_oldTermios));
  #endif
  }

  bool UnixWinTerminal::putChar(char c)
  {
  #ifdef _WIN32
    DWORD written;
    if (!WriteConsole(_hStdout, &c, 1, &written, nullptr) || written != 1)
    {
      _lastError = "Failed to write to console";
      return false;
    }
  #else
    if (write(STDOUT_FILENO, &c, 1) != 1)
    {
      _lastError = "Failed to write to terminal";
      return false;
    }
  #endif
    return true;
  }

  bool UnixWinTerminal::getChar(char& c)
  {
  #ifdef _WIN32
    DWORD read;
    char buf;
    if (!ReadFile(_hStdin, &buf, 1, &read, nullptr) || read != 1)
    {
      _lastError = "Failed to read from console";
      return false;
    }
    c = buf;
  #else
    if (read(STDIN_FILENO, &c, 1) != 1)
    {
      _lastError = "Failed to read from terminal";
      return false;
    }
  #endif
    return true;
  }

  bool UnixWinTerminal::getCharTimeout(char& c, uint32_t timeout_ms)
  {
  #ifdef _WIN32
    // Windows implementation using WaitForSingleObject
    DWORD result = WaitForSingleObject(_hStdin, timeout_ms);
    if (result == WAIT_OBJECT_0) {
      return getChar(c);
    }

    return false;
  #else
    // Unix implementation using select
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int result = select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv);

    if (result > 0) {
      return getChar(c);
    }

    return false;
  #endif
  }

  bool UnixWinTerminal::available() const
  {
  #ifdef _WIN32
    DWORD events;
    GetNumberOfConsoleInputEvents(_hStdin, &events);
    return events > 0;
  #else
    int bytes;
    ioctl(STDIN_FILENO, FIONREAD, &bytes);
    return bytes > 0;
  #endif
  }

  void UnixWinTerminal::flush()
  {
  #ifdef _WIN32
    FlushConsoleInputBuffer(_hStdin);
  #else
    tcflush(STDIN_FILENO, TCIFLUSH);
  #endif
  }

  bool UnixWinTerminal::isOpen() const {
    return _isOpen;
  }

  bool UnixWinTerminal::hasError() const {
    return !_lastError.empty();
  }

  const char* UnixWinTerminal::getLastError() const {
    return _lastError.c_str();
  }

  void UnixWinTerminal::clearError() {
    _lastError.clear();
  }

}
