#pragma once

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <memory>
#include <algorithm>
#include <stdexcept>

class CommandRequest {
public:
  enum class Type {
    Navigation,
    Execution,
    RootNavigation
  };

  CommandRequest(const std::string& input);
  
  const std::vector<std::string>& getPath() const { return _path; }
  const std::vector<std::string>& getArgs() const { return _args; }
  const std::string& getCommandName() const { return _commandName; }
  Type getType() const { return _type; }
  bool isAbsolute() const { return _absolute; }

private:
  std::vector<std::string> _path;
  std::vector<std::string> _args;
  std::string _commandName;
  bool _absolute;
  Type _type;

  void parseInput(const std::string& input);
  static std::vector<std::string> splitPath(const std::string& path);
};
