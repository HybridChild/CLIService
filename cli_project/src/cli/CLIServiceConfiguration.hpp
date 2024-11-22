#pragma once

#include <memory>
#include <unordered_map>
#include <string>

#include "../menu/CommandMenuTree.hpp"
#include "../io/IOStreamIf.hpp"
#include "../user/User.hpp"

class CLIServiceConfiguration {
public:
  CLIServiceConfiguration(
    std::unique_ptr<CommandMenuTree> menuTree,
    std::unique_ptr<IOStreamIf> ioStream,
    std::unordered_map<std::string, User> users
  ) : _menuTree(std::move(menuTree)), _ioStream(std::move(ioStream)), _users(std::move(users)) {}

  CommandMenuTree* getMenuTree() const { return _menuTree.get(); }
  IOStreamIf* getIOStream() const { return _ioStream.get(); }
  
  const User* authenticateUser(const std::string& username, const std::string& password) const {
    auto it = _users.find(username);
    if (it != _users.end() && it->second.getPassword() == password) {
      return &it->second;
    }
    return nullptr;
  }

private:
  std::unique_ptr<CommandMenuTree> _menuTree;
  std::unique_ptr<IOStreamIf> _ioStream;
  std::unordered_map<std::string, User> _users;
};
