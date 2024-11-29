#include "factory/CommandMenuTreeFactory.hpp"
#include "commands/RgbLedCommand.hpp"
#include "commands/PotmeterCommand.hpp"
#include "commands/GetAnalyticsCommand.hpp"
#include "commands/AccessLevel.hpp"

namespace cliService {

  std::unique_ptr<CommandMenuTree> CommandMenuTreeFactory::createDefaultTree() {
    auto tree = std::make_unique<CommandMenuTree>();
    MenuNode* root = tree->getRoot();

    // Get branch
    MenuNode* getNode = root->addSubMenu("get", AccessLevel::Basic);
    MenuNode* getHwNode = getNode->addSubMenu("hw", AccessLevel::Basic);
    getHwNode->addCommand(std::make_unique<PotmeterCommand>("potmeter", AccessLevel::Basic));
    MenuNode* getAnalyticsNode = getNode->addSubMenu("analytics", AccessLevel::Admin);
    getAnalyticsNode->addCommand(std::make_unique<GetAnalyticsCommand>("getData", AccessLevel::Admin));

    // Set branch
    MenuNode* setNode = root->addSubMenu("set", AccessLevel::Advanced);
    MenuNode* setHwNode = setNode->addSubMenu("hw", AccessLevel::Advanced);
    setHwNode->addCommand(std::make_unique<RgbLedCommand>("rgbLed", AccessLevel::Advanced));
    
    return tree;
  }

}
