#pragma once
#include <string>
#include <vector>
#include <cstddef>

namespace cliService
{

  class CommandHistory
  {
  public:
    CommandHistory(size_t maxSize)
      : _maxSize(maxSize)
      , _currentIndex(0)
    {}

    void addCommand(const std::string& command)
    {
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

    std::string getPreviousCommand()  // triggered by UP arrow
    {
      if (_history.empty()) {
        return "";
      }

      if (_currentIndex > 0) {
        _currentIndex -= 1;
      }

      return _history[_currentIndex];
    }

    std::string getNextCommand()  // triggered by DOWN arrow
    {
      if (_history.empty() || _currentIndex >= _history.size()) {
        return "";
      }

      _currentIndex++;
      
      if (_currentIndex >= _history.size()) {
        return "";
      }

      return _history[_currentIndex];
    }

    void resetNavigation()  // called when Enter is pressed
    {
      _currentIndex = _history.size();
    }
    
    void clear()
    {
      _history.clear();
      _currentIndex = 0;
    }

    size_t size() const { return _history.size(); }

    // Get current navigation index (for testing)
    size_t getCurrentIndex() const {
      return _currentIndex;
    }

  private:
    std::vector<std::string> _history;
    size_t _maxSize;
    size_t _currentIndex;
  };

}
