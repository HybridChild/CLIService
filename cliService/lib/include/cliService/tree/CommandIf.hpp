#pragma once

#include "cliService/tree/NodeIf.hpp"
#include <vector>

namespace cliService
{

  class CommandIf : public NodeIf
  {
  public:
    explicit CommandIf(std::string name);
    
    bool isDirectory() const override;
    virtual void execute(const std::vector<std::string>& args) = 0;
  };

}
