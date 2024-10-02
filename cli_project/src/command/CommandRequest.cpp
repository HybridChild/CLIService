#include "CommandRequest.hpp"

CommandRequest::CommandRequest(const std::string& input) {
  parseInput(input);
}

void CommandRequest::setResponse(const std::string& resp, int code) {
  response = resp;
  responseCode = code;
}

void CommandRequest::parseInput(const std::string& input) {
  if (input.empty() || input == "/") {
    type = Type::RootNavigation;
    absolute = true;
    return;
  }

  absolute = input[0] == '/';
  std::string processedInput = absolute ? input.substr(1) : input;

  if (processedInput.empty()) {
    type = Type::RootNavigation;
    return;
  }

  if (processedInput.back() == '/' || processedInput == "..") {
    type = Type::Navigation;
    path = splitPath(processedInput);
  } else {
    type = Type::Execution;
    std::vector<std::string> segments = splitPath(processedInput);
    
    if (!segments.empty()) {
      commandName = segments.back();
      segments.pop_back();
      path = segments;

      // Parse arguments
      std::istringstream iss(commandName);
      iss >> commandName;
      std::string arg;
      while (iss >> arg) {
        args.push_back(arg);
      }
    }
  }

  // // Debug output (you can remove this in production)
  // std::cout << "Debug: Command parsed as:" << std::endl;
  // std::cout << "  Type: " << (type == Type::Navigation ? "Navigation" : 
  //                             type == Type::Execution ? "Execution" : "RootNavigation") << std::endl;
  // std::cout << "  Absolute: " << (absolute ? "Yes" : "No") << std::endl;
  // std::cout << "  Path: ";
  // for (const auto& p : path) std::cout << p << "/";
  // std::cout << std::endl;
  // std::cout << "  Command: " << commandName << std::endl;
  // std::cout << "  Args: ";
  // for (const auto& a : args) std::cout << a << " ";
  // std::cout << std::endl;
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
