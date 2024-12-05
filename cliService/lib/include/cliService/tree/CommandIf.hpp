#pragma once

#include "cliService/tree/NodeIf.hpp"
#include <vector>

namespace cliService
{

  class CommandIf : public NodeIf
  {
  public:
    explicit CommandIf(std::string name, AccessLevel level)
      : NodeIf(std::move(name), level)
    {}
    
    bool isDirectory() const override { return false; };
    virtual void execute(const std::vector<std::string>& args) = 0;
  };

}
