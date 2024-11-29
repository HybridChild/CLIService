#include "cliService/MenuNode.hpp"

namespace cliService {

  MenuNode* MenuNode::addSubMenu(const std::string& name, AccessLevel accessLevel) {
    auto [it, inserted] = _subMenus.try_emplace(name, std::make_unique<MenuNode>(name, accessLevel, this));
    return it->second.get();
  }

  void MenuNode::addCommand(std::unique_ptr<Command> command) {
    _commands[command->getName()] = std::move(command);
  }

  MenuNode* MenuNode::getSubMenu(const std::string& name) const {
    auto it = _subMenus.find(name);
    return (it != _subMenus.end()) ? it->second.get() : nullptr;
  }

  Command* MenuNode::getCommand(const std::string& name) const {
    auto it = _commands.find(name);
    return (it != _commands.end()) ? it->second.get() : nullptr;
  }

}
