#include <sstream>

#include "CommandRequest.hpp"

namespace cliService {

  CommandRequest::CommandRequest(const std::string& input) {
    parseInput(input);
  }

  void CommandRequest::parseInput(const std::string& input) {
    if (input.empty() || input == "/") {
      _type = Type::RootNavigation;
      _absolute = true;
      return;
    }

    _absolute = input[0] == '/';
    std::string processedInput = _absolute ? input.substr(1) : input;

    if (processedInput.empty()) {
      _type = Type::RootNavigation;
      return;
    }

    if (processedInput.back() == '/' || processedInput == "..") {
      _type = Type::Navigation;
      _path = splitPath(processedInput);
    }
    else {
      _type = Type::Execution;
      std::vector<std::string> segments = splitPath(processedInput);
      
      if (!segments.empty()) {
        _commandName = segments.back();
        segments.pop_back();
        _path = segments;

        // Parse arguments
        std::istringstream iss(_commandName);
        iss >> _commandName;
        std::string arg;
        
        while (iss >> arg) {
          _args.push_back(arg);
        }
      }
    }
  }

  std::vector<std::string> CommandRequest::splitPath(const std::string& path) {
    std::vector<std::string> segments;
    std::istringstream iss(path);
    std::string segment;

    while (std::getline(iss, segment, '/')) {
      if (!segment.empty() || segments.empty()) {
        segments.push_back(segment);
      }
    }

    return segments;
  }

}
