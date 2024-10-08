#pragma once

#include "../command/CommandRequest.hpp"
#include "MenuNode.hpp"

class CommandMenuTree {
public:
  CommandMenuTree();

  MenuNode* getRoot();
  std::string getPath(const MenuNode* node) const;

private:
  MenuNode _root;
};
