#pragma once
#include "cliService/cli/RequestBase.hpp"
#include "cliService/tree/Path.hpp"
#include <string>
#include <vector>
#include <cassert>
#include <sstream>
#include <istream>


namespace cliService
{

  class CommandRequest : public RequestBase
  {
  public:
    explicit CommandRequest(Path path, std::vector<std::string> args)
      : _path(std::move(path))
      , _args(std::move(args))
    {}

    const Path& getPath() const { return _path; }
    const std::vector<std::string>& getArgs() const { return _args; }

  private:
    Path _path;
    std::vector<std::string> _args;
  };

}
