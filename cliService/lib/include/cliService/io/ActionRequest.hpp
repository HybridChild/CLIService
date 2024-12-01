#pragma once

#include "cliService/io/RequestBase.hpp"
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
      ArrowUp,    // Command history navigation
      ArrowDown,
      // Add more triggers as needed
    };
    
    ActionRequest(std::string inputStr, Trigger trigger);

    const std::vector<std::string>& getPath() const;
    const std::vector<std::string>& getArgs() const;
    bool isAbsolutePath() const;
    Trigger getTrigger() const;

  private:
    std::vector<std::string> _path;
    std::vector<std::string> _args;
    bool _absolutePath;
    Trigger _trigger;
  };

}
