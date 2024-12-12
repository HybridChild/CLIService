#pragma once
#include <string>
#include <vector>
#include <cstddef>

namespace cliService
{

  class CommandHistory
  {
  public:
    explicit CommandHistory(size_t maxSize = 50);

    void addCommand(const std::string& command);

    std::string getPreviousCommand();  // triggered by UP arrow
    std::string getNextCommand();  // triggered by DOWN arrow
    void resetNavigation();  // called when Enter is pressed

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
