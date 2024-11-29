#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include "CommandIf.hpp"

namespace cliService {

  enum class AccessLevel;

  class MenuNode {
  public:
    MenuNode(const std::string& name, AccessLevel accessLevel, MenuNode* parent = nullptr)
      : _name(name), _accessLevel(accessLevel), _parent(parent)
    {}

    MenuNode* addSubMenu(const std::string& name, AccessLevel accessLevel);
    void addCommand(std::unique_ptr<Command> command);
    MenuNode* getSubMenu(const std::string& name) const;
    Command* getCommand(const std::string& name) const;

    std::string getName() const { return _name; }
    MenuNode* getParent() const { return _parent; }
    AccessLevel getAccessLevel() const { return _accessLevel; }

    const std::unordered_map<std::string, std::unique_ptr<MenuNode>>& getSubMenus() const { return _subMenus; }
    const std::unordered_map<std::string, std::unique_ptr<Command>>& getCommands() const { return _commands; }

  private:
    std::string _name;
    AccessLevel _accessLevel;
    MenuNode* _parent;

    std::unordered_map<std::string, std::unique_ptr<MenuNode>> _subMenus;
    std::unordered_map<std::string, std::unique_ptr<Command>> _commands;
  };
  
}
