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
    std::unordered_map<std::string, User> users,
    const std::string& indent = "  ",
    const std::string& logInPrompt = "Logged out. Please enter <username>:<password>\n > ",
    const std::string& welcomeMessage = "Welcome to the CLI Service. Type 'exit' to quit. Type '?' for info.\n",
    const std::string& exitString = "Thank you for using the CLI Service. Goodbye!\n"
  )
    : _menuTree(std::move(menuTree))
    , _ioStream(std::move(ioStream))
    , _users(std::move(users))
    , _indent(indent)
    , _logInPrompt(logInPrompt)
    , _welcomeMessage(welcomeMessage)
    , _exitString(exitString)
  {}

  CommandMenuTree* getMenuTree() const { return _menuTree.get(); }
  IOStreamIf* getIOStream() const { return _ioStream.get(); }

  std::string getIndent(uint32_t count = 1) const {
    std::string returnStr{};
    for (uint32_t i = 0; i < count; ++i) {
      returnStr += _indent;
    }
    return returnStr;
  }
  
  const User* authenticateUser(const std::string& username, const std::string& password) const {
    auto it = _users.find(username);
    if (it != _users.end() && it->second.getPassword() == password) {
      return &it->second;
    }
    return nullptr;
  }

  const std::string& getLogInPrompt() {
    return _logInPrompt;
  }

  const std::string& getWelcomeMessage() {
    return _welcomeMessage;
  }

  const std::string& getExitString() {
    return _exitString;
  }


private:
  std::unique_ptr<CommandMenuTree> _menuTree;
  std::unique_ptr<IOStreamIf> _ioStream;
  std::unordered_map<std::string, User> _users;
  const std::string _indent;
  const std::string _logInPrompt;
  const std::string _welcomeMessage;
  const std::string _exitString;
};
