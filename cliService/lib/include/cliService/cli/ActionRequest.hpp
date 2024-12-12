#pragma once
#include "cliService/cli/RequestBase.hpp"
#include "cliService/tree/Path.hpp"
#include <string>
#include <vector>

namespace cliService
{

  class ActionRequest : public RequestBase
  {
  public:

    enum class Trigger
    {
      Enter,
      Tab,
      ArrowUp,
      ArrowDown,
      ArrowLeft,
      ArrowRight
    };

    explicit ActionRequest(std::string_view inputStr, Trigger trigger);
    
    const Path& getPath() const { return _path; }
    const std::vector<std::string>& getArgs() const { return _args; }
    Trigger getTrigger() const { return _trigger; }

  private:
    Path _path;
    std::vector<std::string> _args;
    Trigger _trigger;

    static void parseInput(std::string_view input, std::string_view& pathStr, std::string_view& argsStr);
  };

}
