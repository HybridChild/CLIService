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
  
  const std::vector<std::string>& getPath() const { return _path; }
  const std::vector<std::string>& getArgs() const { return _args; }
  const std::string& getCommandName() const { return _commandName; }
  Type getType() const { return _type; }
  const std::string& getResponse() const { return _response; }
  int getResponseCode() const { return _responseCode; }
  void setResponse(const std::string& resp, int code = 0);
  bool isAbsolute() const { return _absolute; }

private:
  std::vector<std::string> _path;
  std::vector<std::string> _args;
  std::string _commandName;
  std::string _response;
  int _responseCode = 0;
  bool _absolute;
  Type _type;

  void parseInput(const std::string& input);
  std::vector<std::string> splitPath(const std::string& path);
};
