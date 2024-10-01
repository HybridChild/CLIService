#include "MenuNode.hpp"

MenuNode::MenuNode(const std::string& name, MenuNode* parent)
  : name(name), parent(parent) {}

MenuNode* MenuNode::addSubMenu(const std::string& name) {
  auto [it, inserted] = subMenus.try_emplace(name, std::make_unique<MenuNode>(name, this));
  return it->second.get();
}

void MenuNode::addCommand(std::unique_ptr<Command> command) {
  commands[command->getName()] = std::move(command);
}

MenuNode* MenuNode::getSubMenu(const std::string& name) {
  auto it = subMenus.find(name);
  return (it != subMenus.end()) ? it->second.get() : nullptr;
}

Command* MenuNode::getCommand(const std::string& name) {
  auto it = commands.find(name);
  return (it != commands.end()) ? it->second.get() : nullptr;
}

std::string MenuNode::getName() const { return name; }
MenuNode* MenuNode::getParent() const { return parent; }

const std::unordered_map<std::string, std::unique_ptr<MenuNode>>& MenuNode::getSubMenus() const { return subMenus; }
const std::unordered_map<std::string, std::unique_ptr<Command>>& MenuNode::getCommands() const { return commands; }
