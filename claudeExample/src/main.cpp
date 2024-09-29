#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <memory>

// Forward declarations
class Command;
class MenuNode;
class CommandMenuTree;
class CommandActions;

// CommandRequest class
class CommandRequest {
public:
  CommandRequest(const std::string& input) {
    parseInput(input);
  }
  
  const std::vector<std::string>& getPath() const { return path; }
  const std::vector<std::string>& getArgs() const { return args; }
  void setResponse(const std::string& resp, int code = 0) { 
    response = resp; 
    responseCode = code;
  }
  const std::string& getResponse() const { return response; }
  int getResponseCode() const { return responseCode; }

private:
  std::vector<std::string> path;
  std::vector<std::string> args;
  std::string response;
  int responseCode = 0;

  void parseInput(const std::string& input) {
    std::istringstream iss(input);
    std::string token;
    
    // Parse the command path
    while (std::getline(iss, token, '/')) {
      if (!token.empty()) {
        path.push_back(token);
      }
    }
    
    // If there are arguments, the last token will contain them
    if (!path.empty()) {
      std::string lastToken = path.back();
      path.pop_back();
      
      std::istringstream argStream(lastToken);
      std::string arg;
      bool firstArg = true;
      
      while (argStream >> arg) {
        if (firstArg) {
          path.push_back(arg);
          firstArg = false;
        } else {
          args.push_back(arg);
        }
      }
    }
  }
};

// Command class (unchanged)
class Command {
public:
  Command(const std::string& name, const std::function<void(CommandRequest&)>& action)
    : name(name), action(action) {}
  
  void execute(CommandRequest& request) {
    action(request);
  }

  std::string getName() const { return name; }

private:
  std::string name;
  std::function<void(CommandRequest&)> action;
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

  void addCommand(const std::string& name, const std::function<void(CommandRequest&)>& action) {
    commands[name] = std::make_unique<Command>(name, action);
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

// Updated CommandMenuTree class
class CommandMenuTree {
public:
  CommandMenuTree() : root("root"), currentNode(&root) {
    setupCommandTree();
  }

  MenuNode* getCurrentNode() {
    return currentNode;
  }

  MenuNode* getRoot() {
    return &root;
  }

  void navigate(const std::string& path) {
    if (path == "..") {
      // Move up one level
      if (currentNode != &root && currentNode->getParent() != nullptr) {
        currentNode = currentNode->getParent();
      }
    } else {
      MenuNode* node = currentNode->getSubMenu(path);
      if (node) {
        currentNode = node;
      } else {
        std::cout << "Invalid path: " << path << std::endl;
      }
    }
  }

  void executeCommand(CommandRequest& request) {
    MenuNode* node = &root;
    const auto& path = request.getPath();
    
    // Navigate to the correct node
    for (size_t i = 0; i < path.size() - 1; ++i) {
      node = node->getSubMenu(path[i]);
      if (!node) {
        request.setResponse("Error: Invalid path", 1);
        return;
      }
    }

    // Execute the command
    if (!path.empty()) {
      Command* cmd = node->getCommand(path.back());
      if (cmd) {
        cmd->execute(request);
      } else {
        request.setResponse("Error: Command not found", 1);
      }
    } else {
      request.setResponse("Error: No command specified", 1);
    }
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

  void setupCommandTree() {
    // Set up the command tree structure
    root.addSubMenu("get");
    root.addSubMenu("set");

    MenuNode* getNode = root.getSubMenu("get");
    getNode->addSubMenu("hw");

    MenuNode* getHwNode = getNode->getSubMenu("hw");
    getHwNode->addCommand("potmeter", [](CommandRequest& req) {
      // Simulate reading potmeter value
      req.setResponse("Potmeter value: 512", 0);
    });

    MenuNode* setNode = root.getSubMenu("set");
    setNode->addSubMenu("hw");

    MenuNode* setHwNode = setNode->getSubMenu("hw");
    setHwNode->addCommand("rgbLed", [](CommandRequest& req) {
      const auto& args = req.getArgs();
      if (args.size() != 3) {
        req.setResponse("Error: rgbLed command requires 3 arguments", 1);
      } else {
        // Simulate setting RGB LED
        req.setResponse("RGB LED set to " + args[0] + " " + args[1] + " " + args[2], 0);
      }
    });
  }
};

// New CommandActions class
class CommandActions {
public:
  static void getPotmeterValue(CommandRequest& req) {
    // Simulate reading potmeter value
    req.setResponse("Potmeter value: 512", 0);
  }

  static void setRgbLed(CommandRequest& req) {
    const auto& args = req.getArgs();
    if (args.size() != 3) {
      req.setResponse("Error: rgbLed command requires 3 arguments", 1);
    } else {
      // Simulate setting RGB LED
      req.setResponse("RGB LED set to " + args[0] + " " + args[1] + " " + args[2], 0);
    }
  }

  static void showHelp(CommandRequest& req) {
    req.setResponse("Available commands: get/hw/potmeter, set/hw/rgbLed", 0);
  }

  static void exit(CommandRequest& req) {
    req.setResponse("Exiting...", 0);
    // You might want to set a flag or use some other mechanism to signal the main loop to exit
  }
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
    getHwNode->addCommand("potmeter", CommandActions::getPotmeterValue);

    // Set branch
    MenuNode* setNode = root->addSubMenu("set");
    MenuNode* setHwNode = setNode->addSubMenu("hw");
    setHwNode->addCommand("rgbLed", CommandActions::setRgbLed);

    // Root commands
    root->addCommand("help", CommandActions::showHelp);
    root->addCommand("exit", CommandActions::exit);

    return tree;
  }

  static std::unique_ptr<CommandMenuTree> createMinimalTree() {
    auto tree = std::make_unique<CommandMenuTree>();
    MenuNode* root = tree->getRoot();

    root->addCommand("help", CommandActions::showHelp);
    root->addCommand("exit", CommandActions::exit);

    return tree;
  }
};


// Updated CLIService class
class CLIService {
public:
  CLIService(std::unique_ptr<CommandMenuTree> tree) 
    : menuTree(std::move(tree)) {}

  void processCommand(const std::string& input) {
    CommandRequest request(input);
    menuTree->executeCommand(request);
    // Here you would send the response back through the serial port
    std::cout << "Response: " << request.getResponse() << " (Code: " << request.getResponseCode() << ")" << std::endl;
  }

  void navigate(const std::string& path) {
    menuTree->navigate(path);
  }

  void listCurrentCommands() {
    std::cout << "Current location: " << menuTree->getCurrentPath() << std::endl;
    std::cout << "Available commands:" << std::endl;
    for (const auto& cmd : menuTree->getCurrentNode()->getCommands()) {
      std::cout << "  " << cmd.first << std::endl;
    }
    std::cout << "Available submenus:" << std::endl;
    for (const auto& submenu : menuTree->getCurrentNode()->getSubMenus()) {
      std::cout << "  " << submenu.first << "/" << std::endl;
    }
  }

private:
  std::unique_ptr<CommandMenuTree> menuTree;
};

int main() {
  // Create a CLIService with the default command tree
  CLIService cli(CommandMenuTreeFactory::createDefaultTree());

  // Example usage
  cli.processCommand("set/hw/rgbLed 255 55 123");
  cli.processCommand("get/hw/potmeter");
  cli.processCommand("help");
  cli.listCurrentCommands();

  // If you want to use a minimal tree instead, you can do:
  // CLIService minimalCli(CommandMenuTreeFactory::createMinimalTree());

  return 0;
}