#pragma once

#include "CommandMenuTree.hpp"

class CommandMenuTreeFactory {
public:
  static std::unique_ptr<CommandMenuTree> createDefaultTree();
};
