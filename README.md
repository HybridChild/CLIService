# CLI Service

A lightweight C++ library for adding a flexible command-line interface to embedded devices.

## Features
- Unix-like shell experience
- Tab completion and command history with arrow key navigation
- Composite tree structure of directories and commands
- User authentication with password protection (Password masking during login)
- Assign access levels to directories and commands
- Allows for both static and dynamic allocation of directories and commands
- Minimal dependencies

## Quick Start
```cpp
// First define your access levels (in a header file)
enum class AccessLevel {
  User,
  Admin
};

// Provide platform specific implementation of the CharIOStream interface
class MyCharIOStream : public CharIOStreamIf
{
public:
  bool putChar(char c) override { /* Write to your hardware */ }
  bool getChar(char& c) override { /* Read from your hardware */ }
  bool getCharTimeout(char& c, uint32_t timeout_ms) override { /* Read with timeout */ }
  bool available() const override { /* Check if data available */ }
  void flush() override { /* Clear input buffer */ }
  bool isOpen() const override { return true; }
  bool hasError() const override { return false; }
  const char* getLastError() const override { return ""; }
  void clearError() override {}
};

// Implement custom commands
class MyCommand : public CommandIf
{
public:
  MyCommand(std::string name, AccessLevel level, std::string description = "")
    : CommandIf(std::move(name), level, std::move(description))
  {}

  CLIResponse execute(const std::vector<std::string>& args) override
  {
    if (!args.empty()) {
      return createInvalidArgumentCountResponse(0);
    }

    // Perform command logic here
    return CLIResponse::success("Command executed successfully.");
  }
};

// Application entry point
int main()
{
  // Create I/O stream
  MyCharIOStream ioStream{};

  // Define users with access levels
  std::vector<User> users{
    {"admin", "admin123", AccessLevel::Admin},
    {"user", "user123", AccessLevel::User}
  };

  // Create menu structure with static and/or dynamic allocation

  // Dynamic root
  auto rootDir = std::make_unique<Directory>("root", AccessLevel::User);

  // Static nodes
  static Directory sysDir("system", AccessLevel::Admin);
  static MyCommand rebootCmd("reboot", AccessLevel::Admin, "Reboot the device");

  // Add static directory to dynamic root
  rootDir->addStaticDirectory(sysDir);
    
  // Add static command to static directory
  sysDir.addStaticCommand(rebootCmd);
    
  // Add dynamic command to static directory
  sysDir.addDynamicCommand<MyCommand>("heap", AccessLevel::User, "Get heap statistics");
    
  // Add dynamic hardware directory with dynamic command
  auto& hwDir = rootDir->addDynamicDirectory("hw", AccessLevel::User);
  hwDir.addDynamicCommand<MyCommand>("setRgb", AccessLevel::Admin, "Set RGB LED color");

  // Configure and create CLI service
  constexpr uint32_t inputTimeout_ms = 1000;
  constexpr size_t historySize = 10;

  CLIServiceConfiguration config {
    static_cast<CharIOStreamIf&>(ioStream),
    std::move(users),
    std::move(rootDir),
    inputTimeout_ms,
    historySize
  };

  CLIService cli(std::move(config));
  cli.activate();

  // Service loop
  while (cli.getCLIState() != CLIState::Inactive)
  {
    cli.service();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return 0;
}
```

## Building
```bash
# Linux/Mac
sh configureCMake.sh
sh build.sh
sh runTests.sh
sh runExample.sh

# Windows
.\configureCMake.bat
.\build.bat
.\runTests.bat
.\runExample.bat
```

## Requirements
- C++17 compiler
- CMake 3.20+
- Google Test (for building tests)

## Example Session
```

  Welcome to CLI Service. Please login.

> admin:********

  Logged in. Type 'help' for help.

admin@/> help

  help   - List global commands
  tree   - Print directory tree
  ?      - Detail items in current directory
  logout - Exit current session
  clear  - Clear screen
  exit   - Exit the CLI

admin@/> tree

  root/
    system/
      reboot
      heap

admin@/ > system/
admin@/system> ?

  reboot - Reboot the device
  heap - Get heap statistics

admin@/system> reboot

  System reboot initiated...

admin@/system > ..
admin@/> logout

  Logged out.

> 
```
