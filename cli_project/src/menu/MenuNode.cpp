#include "MenuNode.hpp"

MenuNode* MenuNode::addSubMenu(const std::string& name, Command::AccessLevel accessLevel) {
  auto [it, inserted] = subMenus.try_emplace(name, std::make_unique<MenuNode>(name, this, accessLevel));
  return it->second.get();
}

void MenuNode::addCommand(std::unique_ptr<Command> command) {
  commands[command->getName()] = std::move(command);
}

MenuNode* MenuNode::getSubMenu(const std::string& name) const {
  auto it = subMenus.find(name);
  return (it != subMenus.end()) ? it->second.get() : nullptr;
}

Command* MenuNode::getCommand(const std::string& name) const {
  auto it = commands.find(name);
  return (it != commands.end()) ? it->second.get() : nullptr;
}