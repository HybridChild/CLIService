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
    explicit CommandRequest(Path path, std::vector<std::string> args, std::string originalInput)
      : _path(std::move(path))
      , _args(std::move(args))
      , _originalInput(std::move(originalInput))
    {}

    explicit CommandRequest(Path path, std::vector<std::string> args, std::string_view originalInput)
      : _path(std::move(path))
      , _args(std::move(args))
      , _originalInput(std::string(originalInput))
    {}

    const Path& getPath() const { return _path; }
    const std::vector<std::string>& getArgs() const { return _args; }
    const std::string& getOriginalInput() const { return _originalInput; }

  private:
    Path _path;
    std::vector<std::string> _args;
    std::string _originalInput;
  };

}
