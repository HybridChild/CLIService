# CLI Service

A lightweight C++ library for adding a flexible command-line interface to embedded devices. Provides a Unix-like shell experience with directories, commands, and tab completion.

## Features
- Hierarchical command structure with directories and nested commands
- Tab completion for paths and commands
- Command history with arrow key navigation
- Customizable user authentication and access levels
- Password masking during login
- Easy to extend with custom commands
- Terminal I/O abstraction for device integration
- Minimal dependencies

## Quick Start
### Concrete terminal implementation for your platform
```cpp
class MyTerminal : public TerminalIf
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
```

### Implement custom commands
```cpp
class RebootCommand : public CommandIf
{
public:
  RebootCommand(std::string name, AccessLevel level, std::string description = "")
    : CommandIf(std::move(name), level, "Reboot the device")
  {}

  CommandResponse execute(const std::vector<std::string>& args) override
  {
    if (args.size() > 0) {
      return CommandResponse("Command take no arguments.", CommandStatus::InvalidArguments);
    }

    // signal system reboot
    return CommandResponse::success("System reboot initiated...");
  }
};
```

### Define your access levels
```cpp
enum class AccessLevel
{
  User,
  Admin
};
```

### Create CLI service
```cpp
// Define users for your application
std::vector<User> users = {
  {"user", "user123", AccessLevel::User},
  {"admin", "admin123", AccessLevel::Admin}
};

// Create menu structure
auto dirRoot = std::make_unique<Directory>("root", AccessLevel::User);
  auto& dirSystem = dirRoot->addDirectory("system", AccessLevel::User);
    dirSystem.addCommand<RebootCommand>("reboot", AccessLevel::Admin);
    dirSystem.addCommand<HeapStatsGetCommand>("heap", AccessLevel::User);

// Create CLI service
MyTerminal terminal{};
CLIServiceConfiguration config{terminal, std::move(users), std::move(dirRoot)};
CLIService cli(std::move(config));

// Run service
cli.activate();
while (true) {
  cli.service();
  // Handle other tasks...
}
```

## Building
```bash
# Configure with CMake
./configure.sh  # or configure.bat on Windows

# Build
./build.sh     # or build.bat on Windows

# Run tests
./runTests.sh  # or runTests.bat on Windows

# Run example
./runExample.sh  # or runExample.bat on Windows
```

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
admin@/system >
```

## Requirements
- C++17 compiler
- CMake 3.20+
- Google Test (for building tests)