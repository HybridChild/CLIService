#include "CommandMenuTreeFactory.hpp"
#include "../commands/RgbLedCommand.hpp"
#include "../commands/PotmeterCommand.hpp"
#include "../commands/GetAnalyticsCommand.hpp"

std::unique_ptr<CommandMenuTree> CommandMenuTreeFactory::createDefaultTree() {
  auto tree = std::make_unique<CommandMenuTree>();
  MenuNode* root = tree->getRoot();

  // Get branch
  MenuNode* getNode = root->addSubMenu("get", Command::AccessLevel::Basic);
  MenuNode* getHwNode = getNode->addSubMenu("hw", Command::AccessLevel::Basic);
  getHwNode->addCommand(std::make_unique<PotmeterCommand>(Command::AccessLevel::Basic));
  MenuNode* getAnalyticsNode = getNode->addSubMenu("analytics", Command::AccessLevel::Admin);
  getAnalyticsNode->addCommand(std::make_unique<GetAnalyticsCommand>(Command::AccessLevel::Admin));

  // Set branch
  MenuNode* setNode = root->addSubMenu("set", Command::AccessLevel::Advanced);
  MenuNode* setHwNode = setNode->addSubMenu("hw", Command::AccessLevel::Advanced);
  setHwNode->addCommand(std::make_unique<RgbLedCommand>(Command::AccessLevel::Advanced));
  
  return tree;
}
