#pragma once

#include "cliService/CommandMenuTree.hpp"

namespace cliService {
  
  class CommandMenuTreeFactory {
  public:
    static std::unique_ptr<CommandMenuTree> createDefaultTree();
  };

}
