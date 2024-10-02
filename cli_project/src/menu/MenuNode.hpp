#pragma once

#include "../command/Command.hpp"
#include <unordered_map>
#include <memory>
#include <string>

class MenuNode {
public:
  MenuNode(const std::string& name, MenuNode* parent = nullptr, Command::AccessLevel accessLevel = Command::AccessLevel::Basic)
    : name(name), parent(parent), accessLevel(accessLevel) {}

  MenuNode* addSubMenu(const std::string& name, Command::AccessLevel accessLevel = Command::AccessLevel::Basic);
  void addCommand(std::unique_ptr<Command> command);
  MenuNode* getSubMenu(const std::string& name) const;
  Command* getCommand(const std::string& name) const;

  std::string getName() const { return name; }
  MenuNode* getParent() const { return parent; }
  Command::AccessLevel getAccessLevel() const { return accessLevel; }

  const std::unordered_map<std::string, std::unique_ptr<MenuNode>>& getSubMenus() const { return subMenus; }
  const std::unordered_map<std::string, std::unique_ptr<Command>>& getCommands() const { return commands; }

private:
  std::string name;
  MenuNode* parent;
  Command::AccessLevel accessLevel;
  std::unordered_map<std::string, std::unique_ptr<MenuNode>> subMenus;
  std::unordered_map<std::string, std::unique_ptr<Command>> commands;
};
