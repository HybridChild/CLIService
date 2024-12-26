#pragma once
#include "cliService/cli/RequestBase.hpp"
#include <string>

namespace cliService
{

  class HistoryNavigationRequest : public RequestBase
  {
  public:
    enum class Direction
    {
      Previous,
      Next
    };

    HistoryNavigationRequest(Direction dir, std::string currentBuffer = "")
      : _direction(dir)
      , _currentBuffer(std::move(currentBuffer))
    {}

    HistoryNavigationRequest(Direction dir, std::string_view currentBuffer = "")
      : _direction(dir)
      , _currentBuffer(currentBuffer)
    {}

    Direction getDirection() const { return _direction; }
    const std::string& getCurrentBuffer() const { return _currentBuffer; }

  private:
    Direction _direction;
    std::string _currentBuffer;
  };

}
