#pragma once

#include "../common.hpp"

class CommandRequest {
public:
  enum class Type {
    Navigation,
    Execution,
    RootNavigation
  };

  CommandRequest(const std::string& input);
  
  const std::vector<std::string>& getPath() const { return path; }
  const std::vector<std::string>& getArgs() const { return args; }
  const std::string& getCommandName() const { return commandName; }
  bool isAbsolute() const { return absolute; }
  Type getType() const { return type; }
  void setResponse(const std::string& resp, int code = 0);
  const std::string& getResponse() const { return response; }
  int getResponseCode() const { return responseCode; }

private:
  std::vector<std::string> path;
  std::vector<std::string> args;
  std::string commandName;
  std::string response;
  int responseCode = 0;
  bool absolute;
  Type type;

  void parseInput(const std::string& input);
  std::vector<std::string> splitPath(const std::string& path);
};
