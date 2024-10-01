#include "CommandMenuTreeFactory.hpp"
#include "command/RgbLedCommand.hpp"
#include "command/PotmeterCommand.hpp"

std::unique_ptr<CommandMenuTree> CommandMenuTreeFactory::createDefaultTree() {
  auto tree = std::make_unique<CommandMenuTree>();
  MenuNode* root = tree->getRoot();

  // Get branch
  MenuNode* getNode = root->addSubMenu("get");
  MenuNode* getHwNode = getNode->addSubMenu("hw");
  getHwNode->addCommand(std::make_unique<PotmeterCommand>());

  // Set branch
  MenuNode* setNode = root->addSubMenu("set");
  MenuNode* setHwNode = setNode->addSubMenu("hw");
  setHwNode->addCommand(std::make_unique<RgbLedCommand>());

  return tree;
}
