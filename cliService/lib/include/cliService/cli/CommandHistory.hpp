#pragma once
#include <string>
#include <vector>
#include <cstddef>

namespace cliService
{

  class CommandHistory
  {
  public:
    explicit CommandHistory(size_t maxSize = 50)
      : _maxSize(maxSize)
      , _currentIndex(0)
    {}

    // Add a new command to history
    void addCommand(const std::string& command) {
      // Don't add empty commands or duplicates of the last command
      if (command.empty() || (!_history.empty() && _history.back() == command)) {
        return;
      }

      if (_history.size() >= _maxSize) {
        _history.erase(_history.begin());
      }

      _history.push_back(command);
      _currentIndex = _history.size();
    }

    // Get previous command (triggered by UP arrow)
    std::string getPreviousCommand() {
      if (_history.empty()) {
        return "";
      }

      if (_currentIndex > 0) {
        _currentIndex--;
      }

      return _history[_currentIndex];
    }

    // Get next command (triggered by DOWN arrow)
    std::string getNextCommand() {
      if (_currentIndex < _history.size() - 1) {
        _currentIndex++;
        return _history[_currentIndex];
      }
      
      // If we reach the end, clear the current command
      _currentIndex = _history.size();
      return "";
    }

    // Reset the history navigation (called when Enter is pressed)
    void resetNavigation() {
      _currentIndex = _history.size();
    }

    // Clear all history
    void clear() {
      _history.clear();
      _currentIndex = 0;
    }

    // Get current number of commands in history
    size_t size() const {
      return _history.size();
    }

    size_t getCurrentIndex() const {
      return _currentIndex;
    }

  private:
    std::vector<std::string> _history;
    size_t _maxSize;
    size_t _currentIndex;
  };

}