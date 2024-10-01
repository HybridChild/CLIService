#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <memory>
#include <algorithm>
#include <stdexcept>

// Forward declarations
class MenuNode;
class CommandMenuTree;

// Updated CommandRequest class with corrected parsing
class CommandRequest {
public:
  CommandRequest(const std::string& input) {
    parseInput(input);
  }
  
  const std::vector<std::string>& getPath() const { return path; }
  const std::vector<std::string>& getArgs() const { return args; }
  const std::string& getCommandName() const { return commandName; }
  void setResponse(const std::string& resp, int code = 0) { 
    response = resp; 
    responseCode = code;
  }
  const std::string& getResponse() const { return response; }
  int getResponseCode() const { return responseCode; }

private:
  std::vector<std::string> path;
  std::vector<std::string> args;
  std::string commandName;
  std::string response;
  int responseCode = 0;

  void parseInput(const std::string& input) {
    std::istringstream iss(input);
    std::string segment;
    std::string remainingInput;
    
    // Parse the path and command name
    while (std::getline(iss, segment, '/')) {
      if (!segment.empty()) {
        path.push_back(segment);
      }
    }
    
    if (!path.empty()) {
      // Extract the last element, which contains the command and possibly arguments
      std::string lastElement = path.back();
      path.pop_back();

      // Split the last element into command and arguments
      std::istringstream lastElementStream(lastElement);
      lastElementStream >> commandName;

      // Parse arguments
      std::string arg;
      while (lastElementStream >> arg) {
        args.push_back(arg);
      }
    }

    // Debug output
    std::cout << "Debug: Command parsed as:" << std::endl;
    std::cout << "  Path: ";
    for (const auto& p : path) std::cout << p << "/";
    std::cout << std::endl;
    std::cout << "  Command: " << commandName << std::endl;
    std::cout << "  Args: ";
    for (const auto& a : args) std::cout << a << " ";
    std::cout << std::endl;
  }
};

// New abstract Command base class
class Command {
public:
  virtual ~Command() = default;
  virtual void execute(CommandRequest& request) = 0;
  virtual std::string getName() const = 0;
  virtual std::string getUsage() const = 0;
};

// Updated RgbLedCommand with more detailed error checking
class RgbLedCommand : public Command {
public:
  void execute(CommandRequest& request) override {
    const auto& args = request.getArgs();
    std::cout << "Debug: RgbLedCommand execute called with " << args.size() << " arguments." << std::endl;
    if (args.size() != 3) {
      request.setResponse("Error: rgbLed command requires 3 arguments. Usage: " + getUsage(), 1);
      return;
    }
    try {
      int r = std::stoi(args[0]);
      int g = std::stoi(args[1]);
      int b = std::stoi(args[2]);
      if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
        throw std::out_of_range("RGB values must be between 0 and 255");
      }
      // Simulate setting RGB LED
      request.setResponse("RGB LED set to " + args[0] + " " + args[1] + " " + args[2], 0);
    } catch (const std::exception& e) {
      request.setResponse("Error: Invalid RGB values. " + std::string(e.what()), 1);
    }
  }

  std::string getName() const override { return "rgbLed"; }
  std::string getUsage() const override { return "rgbLed <red> <green> <blue>"; }
};

// Example of another concrete Command subclass
class PotmeterCommand : public Command {
public:
  void execute(CommandRequest& request) override {
    if (!request.getArgs().empty()) {
      request.setResponse("Error: potmeter command takes no arguments. Usage: " + getUsage(), 1);
      return;
    }
    // Simulate reading potmeter value
    request.setResponse("Potmeter value: 512", 0);
  }

  std::string getName() const override { return "potmeter"; }
  std::string getUsage() const override { return "potmeter"; }
};

// Updated MenuNode class
class MenuNode {
public:
  MenuNode(const std::string& name, MenuNode* parent = nullptr)
    : name(name), parent(parent) {}

  MenuNode* addSubMenu(const std::string& name) {
    auto [it, inserted] = subMenus.try_emplace(name, std::make_unique<MenuNode>(name, this));
    return it->second.get();
  }

  void addCommand(std::unique_ptr<Command> command) {
    commands[command->getName()] = std::move(command);
  }

  MenuNode* getSubMenu(const std::string& name) {
    auto it = subMenus.find(name);
    return (it != subMenus.end()) ? it->second.get() : nullptr;
  }

  Command* getCommand(const std::string& name) {
    auto it = commands.find(name);
    return (it != commands.end()) ? it->second.get() : nullptr;
  }

  std::string getName() const { return name; }
  MenuNode* getParent() const { return parent; }

  const std::unordered_map<std::string, std::unique_ptr<MenuNode>>& getSubMenus() const { return subMenus; }
  const std::unordered_map<std::string, std::unique_ptr<Command>>& getCommands() const { return commands; }

private:
  std::string name;
  MenuNode* parent;
  std::unordered_map<std::string, std::unique_ptr<MenuNode>> subMenus;
  std::unordered_map<std::string, std::unique_ptr<Command>> commands;
};

// Updated CommandMenuTree class with more detailed logging
class CommandMenuTree {
public:
  CommandMenuTree() : root("root"), currentNode(&root) {}

  MenuNode* getCurrentNode() { return currentNode; }
  MenuNode* getRoot() { return &root; }

  void navigate(const std::string& path, bool isAbsolute = false) {
    std::vector<std::string> segments = splitPath(path);
    MenuNode* node = isAbsolute ? &root : currentNode;

    for (const auto& segment : segments) {
      if (segment == "..") {
        if (node != &root && node->getParent() != nullptr) {
          node = node->getParent();
        }
      } else if (!segment.empty()) {
        MenuNode* next = node->getSubMenu(segment);
        if (next) {
          node = next;
        } else {
          std::cout << "Invalid path segment: " << segment << std::endl;
          return;
        }
      }
    }
    currentNode = node;
  }

  bool executeCommand(CommandRequest& request, bool isAbsolute = false) {
    // Store original position
    MenuNode* originalNode = currentNode;

    // Navigate to the command's location
    const auto& path = request.getPath();
    if (!path.empty()) {
      std::string navigation = isAbsolute ? "/" : "";
      for (const auto& segment : path) {
        navigation += segment + "/";
      }
      navigate(navigation, isAbsolute);
    }

    std::cout << "Debug: Executing command at path: " << getCurrentPath() << std::endl;

    // Find and execute the command
    Command* cmd = currentNode->getCommand(request.getCommandName());
    if (cmd) {
      std::cout << "Debug: Found command: " << cmd->getName() << std::endl;
      cmd->execute(request);
      currentNode = originalNode;  // Reset to original position
      return true;
    }

    // If command not found, reset to original position and return false
    std::cout << "Debug: Command not found: " << request.getCommandName() << std::endl;
    currentNode = originalNode;
    return false;
  }

  std::string getCurrentPath() {
    std::vector<std::string> path;
    MenuNode* node = currentNode;
    while (node != &root) {
      path.push_back(node->getName());
      node = node->getParent();
    }
    std::string result = "/";
    for (auto it = path.rbegin(); it != path.rend(); ++it) {
      result += *it + "/";
    }
    return result;
  }

private:
  MenuNode root;
  MenuNode* currentNode;

  std::vector<std::string> splitPath(const std::string& path) {
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
};

// Updated CLIService class
class CLIService {
public:
  CLIService(std::unique_ptr<CommandMenuTree> tree) 
    : menuTree(std::move(tree)) {}

  void processCommand(const std::string& input) {
    if (input.empty()) {
      std::cout << "Empty command. Current location: " << menuTree->getCurrentPath() << std::endl;
      return;
    }

    // Handle root navigation
    if (input == "/") {
      menuTree->navigate("", true);  // Navigate to root
      std::cout << "Navigated to root: " << menuTree->getCurrentPath() << std::endl;
      return;
    }

    bool isAbsolute = input[0] == '/';
    std::string processedInput = isAbsolute ? input.substr(1) : input;

    CommandRequest request(processedInput);

    // Check if this is a navigation command
    if (processedInput.back() == '/') {
      menuTree->navigate(processedInput, isAbsolute);
      std::cout << "Navigated to: " << menuTree->getCurrentPath() << std::endl;
    } else {
      // This is a command execution (with or without navigation)
      bool executed = menuTree->executeCommand(request, isAbsolute);
      if (executed) {
        std::cout << "Response: " << request.getResponse() 
                  << " (Code: " << request.getResponseCode() << ")" << std::endl;
      } else {
        std::cout << "Unknown command. Use 'help' for available commands." << std::endl;
      }
    }
  }

  void listCurrentCommands() {
    std::cout << "Current location: " << menuTree->getCurrentPath() << std::endl;
    std::cout << "Available commands:" << std::endl;
    for (const auto& cmd : menuTree->getCurrentNode()->getCommands()) {
      std::cout << "  " << cmd.first << " - Usage: " << cmd.second->getUsage() << std::endl;
    }
    std::cout << "Available submenus:" << std::endl;
    for (const auto& submenu : menuTree->getCurrentNode()->getSubMenus()) {
      std::cout << "  " << submenu.first << "/" << std::endl;
    }
  }

private:
  std::unique_ptr<CommandMenuTree> menuTree;
};


// Updated CommandMenuTreeFactory class
class CommandMenuTreeFactory {
public:
  static std::unique_ptr<CommandMenuTree> createDefaultTree() {
    auto tree = std::make_unique<CommandMenuTree>();
    MenuNode* root = tree->getRoot();

    // Get branch
    MenuNode* getNode = root->addSubMenu("get");
    MenuNode* getHwNode = getNode->addSubMenu("hw");
    getHwNode->addCommand(std::make_unique<PotmeterCommand>());

    // Set branch
    MenuNode* setNode = root->addSubMenu("set");
    MenuNode* setHwNode = setNode->addSubMenu("hw");
    setHwNode->addCommand(std::make_unique<RgbLedCommand>());

    return tree;
  }
};

int main() {
  CLIService cli(CommandMenuTreeFactory::createDefaultTree());

  // Example usage with debug output
  std::cout << "\nExecuting: set/hw/rgbLed 255 55 123" << std::endl;
  cli.processCommand("set/hw/rgbLed 255 55 123");
  
  std::cout << "\nExecuting: get/hw/potmeter" << std::endl;
  cli.processCommand("get/hw/potmeter");
  
  std::cout << "\nNavigating to root" << std::endl;
  cli.processCommand("/");
  
  std::cout << "\nListing commands at root" << std::endl;
  cli.listCurrentCommands();

  return 0;
}