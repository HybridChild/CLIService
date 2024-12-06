#pragma once
#include "cliService/requests/RequestBase.hpp"
#include <string>
#include <vector>

namespace cliService
{

  class ActionRequest : public RequestBase
  {
  public:
    explicit ActionRequest(std::string inputStr);

    const std::vector<std::string>& getPath() const { return _path; }
    const std::vector<std::string>& getArgs() const { return _args; }
    bool isAbsolutePath() const { return _absolutePath; }

  private:
    std::vector<std::string> _path;
    std::vector<std::string> _args;
    bool _absolutePath;
  };

}
