#include "cliService/cli/CommandHistory.hpp"

namespace cliService
{

  CommandHistory::CommandHistory(size_t maxSize)
    : _maxSize(maxSize)
    , _currentIndex(0)
  {}


  void CommandHistory::addCommand(const std::string& command)
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


  std::string CommandHistory::getPreviousCommand()
  {
    if (_history.empty()) {
      return "";
    }

    if (_currentIndex > 0) {
      _currentIndex -= 1;
    }

    return _history[_currentIndex];
  }


  std::string CommandHistory::getNextCommand()
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


  void CommandHistory::resetNavigation() {
    _currentIndex = _history.size();
  }

}
