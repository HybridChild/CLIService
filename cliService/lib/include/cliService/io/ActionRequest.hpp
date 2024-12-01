#pragma once

#include "cliService/io/RequestBase.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <cassert>

namespace cliService
{
  class ActionRequest : public RequestBase
  {
  public:
    ActionRequest(std::string inputStr);

    const std::vector<std::string>& getPath() const;
    const std::vector<std::string>& getArgs() const;
    bool isAbsolutePath() const;

  private:
    std::vector<std::string> _path;
    std::vector<std::string> _args;
    bool _absolutePath;
  };
}
