# CLI Service

A lightweight C++ library for adding a flexible command-line interface to embedded devices.

## Features
- Unix-like shell experience
- Tab completion and command history with arrow key navigation
- Composite tree structure of directories and commands
- User authentication with password protection (Password masking during login)
- Assign access levels to directories and commands
- Minimal dependencies

## Quick Start
```cpp
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
  MyCommand(
    std::string name,
    AccessLevel level,
    std::string description = "Reboot the device")
    : CommandIf(std::move(name)
    , level
    , std::move(description))
  {}

  Response execute(const std::vector<std::string>& args) override
  {
    if (args.size() > 0) {
      return Response("Command takes no arguments.", CommandStatus::InvalidArguments);
    }

    // Perform command logic here

    return Response::success("Command executed successfully.");
  }
};

// Define your access levels
enum class AccessLevel {
  User,
  Admin
};

// Application entry point
int main()
{
  // Define users for your application and assign passwords and access levels
  std::vector<User> users = {
    {"user", "user123", AccessLevel::User},
    {"admin", "admin123", AccessLevel::Admin}
  };

  // Create menu structure
  auto dirRoot = std::make_unique<Directory>("root", AccessLevel::User);
    auto& dirSystem = dirRoot->addDynamicDirectory("system", AccessLevel::User);
      dirSystem.addDynamicCommand<RebootCommand>("reboot", AccessLevel::Admin);
      dirSystem.addDynamicCommand<HeapStatsGetCommand>("heap", AccessLevel::User);

  // Create and activate CLI service
  MyCharIOStream ioStream{};
  CLIServiceConfiguration config{ioStream, std::move(users), std::move(dirRoot)};
  CLIService cli(std::move(config));
  cli.activate();

  // Periocically service the CLI
  while (true) {
    cli.service();
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
Welcome to CLI Service.
Logged out. Please enter <username>:<password>
 > admin:********
admin@/ > help

Global commands:
  help   - List global commands
  tree   - Show directory structure
  ?      - Show help for available commands in current directory
  logout - Exit current session

admin@/ > tree

root/
  system/
    reboot
    heap

admin@/ > system/
admin@/system > ?

Available commands in current directory:
  reboot - Reboot the device
  heap - Get heap statistics

admin@/system > reboot
System reboot initiated...
admin@/system > ..
admin@/ > logout
Logged out. Please enter <username>:<password>
 > 
```
