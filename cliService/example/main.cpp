#include "cliService/cli/CLIService.hpp"
#include "io/UnixWinCharIOStream.hpp"
#include "commands/AccessLevel.hpp"
#include "menuTree/staticMenuTree.hpp"
#include "menuTree/mixedMenuTree.hpp"
#include <vector>
#include <thread>
#include <chrono>

constexpr size_t commandHistorySize = 10;
constexpr uint32_t inputTimeout_ms = 1000;

using namespace cliService;

// Example usage in main
int main()
{
  UnixWinCharIOStream ioStream{};

  std::vector<User> users
  {
    {"admin", "admin123", AccessLevel::Admin},
    {"user", "user123", AccessLevel::User}
  };

  StaticMenuTree staticTree;  // Using fully static allocation of menu tree
  auto mixedTree = createMixedMenuTree();  // OR using mixed allocation
  
  CLIServiceConfiguration staticConfig
  {
    static_cast<CharIOStreamIf&>(ioStream),
    users,
    staticTree.getRoot(),  // Pass reference instead of pointer
    inputTimeout_ms,
    commandHistorySize
  };
  
  CLIServiceConfiguration mixedConfig
  {
    static_cast<CharIOStreamIf&>(ioStream),
    users,
    std::move(mixedTree),  // This constructor takes unique_ptr
    inputTimeout_ms,
    commandHistorySize
  };

  // Create service with static or mixed configuration
  CLIService cli(std::move(mixedConfig));
  
  cli.activate();

  while (cli.getState() != CLIState::Inactive)
  {
    cli.service();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return 0;
}
