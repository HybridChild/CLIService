#pragma once

#include "cliService/tree/Node.hpp"
#include <vector>

namespace cliService
{

  class Command : public Node
  {
  public:
    explicit Command(std::string name);
    
    bool isDirectory() const override;
    virtual void execute(const std::vector<std::string>& args) = 0;
  };

}
