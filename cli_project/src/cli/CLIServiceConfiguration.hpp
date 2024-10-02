#pragma once

#include "../menu/CommandMenuTree.hpp"
#include "../io/InOutStream.hpp"
#include <memory>
#include <unordered_map>
#include <string>

class CLIServiceConfiguration {
public:
  CLIServiceConfiguration(
    std::unique_ptr<CommandMenuTree> menuTree,
    std::unique_ptr<InOutStream> ioStream,
    std::unordered_map<std::string, std::string> users
  ) : menuTree(std::move(menuTree)), ioStream(std::move(ioStream)), users(std::move(users)) {}

  CommandMenuTree* getMenuTree() const { return menuTree.get(); }
  InOutStream* getIOStream() const { return ioStream.get(); }
  
  bool authenticateUser(const std::string& username, const std::string& password) const {
    auto it = users.find(username);
    return (it != users.end() && it->second == password);
  }

private:
  std::unique_ptr<CommandMenuTree> menuTree;
  std::unique_ptr<InOutStream> ioStream;
  std::unordered_map<std::string, std::string> users;
};
