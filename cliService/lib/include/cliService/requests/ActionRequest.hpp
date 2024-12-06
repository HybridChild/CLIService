#pragma once
#include "cliService/requests/RequestBase.hpp"
#include <string>
#include <vector>

namespace cliService
{

  class ActionRequest : public RequestBase
  {
  public:
    enum class Trigger
    {
      Enter,      // Normal command completion with enter
      Tab,        // Tab completion request
      ArrowUp,
      ArrowDown,
      ArrowLeft,
      ArrowRight,
      // Add more triggers as needed
    };
    
    ActionRequest(std::string inputStr, Trigger trigger);

    const std::vector<std::string>& getPath() const { return _path; }
    const std::vector<std::string>& getArgs() const { return _args; }
    bool isAbsolutePath() const { return _absolutePath; }
    Trigger getTrigger() const { return _trigger; }

  private:
    std::vector<std::string> _path;
    std::vector<std::string> _args;
    bool _absolutePath;
    Trigger _trigger;
  };

}
