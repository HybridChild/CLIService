#pragma once

#include "../command/Command.hpp"
#include <unordered_map>
#include <memory>
#include <string>

class MenuNode {
public:
  MenuNode(const std::string& name, MenuNode* parent = nullptr, Command::AccessLevel accessLevel = Command::AccessLevel::Basic)
    : _name(name), _parent(parent), _accessLevel(accessLevel) {}

  MenuNode* addSubMenu(const std::string& name, Command::AccessLevel accessLevel = Command::AccessLevel::Basic);
  void addCommand(std::unique_ptr<Command> command);
  MenuNode* getSubMenu(const std::string& name) const;
  Command* getCommand(const std::string& name) const;

  std::string getName() const { return _name; }
  MenuNode* getParent() const { return _parent; }
  Command::AccessLevel getAccessLevel() const { return _accessLevel; }

  const std::unordered_map<std::string, std::unique_ptr<MenuNode>>& getSubMenus() const { return _subMenus; }
  const std::unordered_map<std::string, std::unique_ptr<Command>>& getCommands() const { return _commands; }

private:
  std::string _name;
  MenuNode* _parent;
  Command::AccessLevel _accessLevel;
  std::unordered_map<std::string, std::unique_ptr<MenuNode>> _subMenus;
  std::unordered_map<std::string, std::unique_ptr<Command>> _commands;
};
