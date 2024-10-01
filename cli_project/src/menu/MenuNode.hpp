#pragma once

#include "../common.hpp"
#include "../command/Command.hpp"

class MenuNode {
public:
  MenuNode(const std::string& name, MenuNode* parent = nullptr);

  MenuNode* addSubMenu(const std::string& name);
  void addCommand(std::unique_ptr<Command> command);
  MenuNode* getSubMenu(const std::string& name);
  Command* getCommand(const std::string& name);

  std::string getName() const;
  MenuNode* getParent() const;

  const std::unordered_map<std::string, std::unique_ptr<MenuNode>>& getSubMenus() const;
  const std::unordered_map<std::string, std::unique_ptr<Command>>& getCommands() const;

private:
  std::string name;
  MenuNode* parent;
  std::unordered_map<std::string, std::unique_ptr<MenuNode>> subMenus;
  std::unordered_map<std::string, std::unique_ptr<Command>> commands;
};
