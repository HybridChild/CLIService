#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include "cliService/CommandIf.hpp"
#include "cliService/MenuItemIf.hpp"

namespace cliService {

  enum class AccessLevel;

  class MenuNode : public MenuItemIf{
  public:
    MenuNode(const std::string& name, AccessLevel accessLevel, MenuNode* parent = nullptr)
      : MenuItemIf(name, accessLevel), _parent(parent)
    {}

    MenuNode* addSubMenu(const std::string& name, AccessLevel accessLevel);
    void addCommand(std::unique_ptr<CommandIf> command);
    MenuNode* getSubMenu(const std::string& name) const;
    CommandIf* getCommand(const std::string& name) const;

    MenuNode* getParent() const { return _parent; }

    const std::unordered_map<std::string, std::unique_ptr<MenuNode>>& getSubMenus() const { return _subMenus; }
    const std::unordered_map<std::string, std::unique_ptr<CommandIf>>& getCommands() const { return _commands; }

  private:
    MenuNode* _parent;

    std::unordered_map<std::string, std::unique_ptr<MenuNode>> _subMenus;
    std::unordered_map<std::string, std::unique_ptr<CommandIf>> _commands;
  };
  
}
