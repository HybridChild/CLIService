#pragma once

#include "../menu/CommandMenuTree.hpp"

class CommandMenuTreeFactory {
public:
  static std::unique_ptr<CommandMenuTree> createDefaultTree();
};
