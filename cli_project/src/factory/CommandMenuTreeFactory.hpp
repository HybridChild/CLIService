#pragma once

#include "../menu/CommandMenuTree.hpp"

namespace cliService {
  class CommandMenuTreeFactory {
  public:
    static std::unique_ptr<CommandMenuTree> createDefaultTree();
  };

}
