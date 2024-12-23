#pragma once
#include "cliService/cli/RequestBase.hpp"
#include "cliService/tree/Path.hpp"

namespace cliService
{

  class TabCompletionRequest : public RequestBase
  {
  public:
    TabCompletionRequest(Path path)
      : _path(std::move(path))
    {}

    const Path& getPath() const { return _path; }

  private:
    Path _path;
  };

}
