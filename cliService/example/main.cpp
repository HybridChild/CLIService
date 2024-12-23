#include "io/UnixWinCharIOStream.hpp"
#include "commands/AccessLevel.hpp"
#include "commands/system/RebootCommand.hpp"
#include "commands/system/HeapStatsGetCommand.hpp"
#include "commands/hw/RgbLedSetCommand.hpp"
#include "commands/hw/PotmeterGetCommand.hpp"
#include "commands/hw/ToggleSwitchGetCommand.hpp"
#include "cliService/cli/CLIService.hpp"
#include <vector>
#include <thread>
#include <chrono>

constexpr size_t commandHistorySize = 10;
constexpr uint32_t inputTimeout_ms = 1000;

using namespace cliService;


// Example of static allocation
class StaticMenuTree
{
public:
  StaticMenuTree()
    : _rootDir("root", AccessLevel::User)
    , _sysDir("system", AccessLevel::Admin)
    , _hwDir("hw", AccessLevel::User)
    , _hwPotDir("pot", AccessLevel::User)
    , _hwToggleDir("toggle", AccessLevel::User)
    , _hwRgbDir("rgb", AccessLevel::User)
    , _rebootCmd("reboot", AccessLevel::Admin)
    , _heapCmd("heap", AccessLevel::Admin)
    , _potmeterCmd("get", AccessLevel::User)
    , _rgbLedCmd("set", AccessLevel::Admin)
    , _toggleSwitchCmd("get", AccessLevel::User)
  {
    // Build tree with static references
    _rootDir.addStaticDirectory(_sysDir);
      _sysDir.addStaticCommand(_rebootCmd);
      _sysDir.addStaticCommand(_heapCmd);

    _rootDir.addStaticDirectory(_hwDir);
      _hwDir.addStaticDirectory(_hwPotDir);
        _hwPotDir.addStaticCommand(_potmeterCmd);
      _hwDir.addStaticDirectory(_hwRgbDir);
        _hwRgbDir.addStaticCommand(_rgbLedCmd);
      _hwDir.addStaticDirectory(_hwToggleDir);
        _hwToggleDir.addStaticCommand(_toggleSwitchCmd);
  }

  Directory& getRoot() { return _rootDir; }

private:
  Directory _rootDir;
  Directory _sysDir;
  Directory _hwDir;
  Directory _hwPotDir;
  Directory _hwToggleDir;
  Directory _hwRgbDir;
  RebootCommand _rebootCmd;
  HeapStatsGetCommand _heapCmd;
  PotmeterGetCommand _potmeterCmd;
  RgbLedSetCommand _rgbLedCmd;
  ToggleSwitchGetCommand _toggleSwitchCmd;
};

// Example of mixed allocation
std::unique_ptr<Directory> createMixedMenuTree()
{
  // Create dynamic root
  auto dirRoot = std::make_unique<Directory>("root", AccessLevel::User);
    
  // Add static system directory
  static Directory sysDir("system", AccessLevel::Admin);
  dirRoot->addStaticDirectory(sysDir);
    
  // Add dynamic commands to static directory
  sysDir.addDynamicCommand<RebootCommand>("reboot", AccessLevel::Admin);
  sysDir.addDynamicCommand<HeapStatsGetCommand>("heap", AccessLevel::Admin);
    
  // Add dynamic hardware directory with dynamic command
  auto& hwDir = dirRoot->addDynamicDirectory("hw", AccessLevel::User);
  hwDir.addDynamicCommand<RgbLedSetCommand>("set", AccessLevel::Admin);
    
  return dirRoot;
}

// Example usage in main
int main()
{
  UnixWinCharIOStream ioStream{};

  // Using fully static allocation
  StaticMenuTree staticTree;
  
  CLIServiceConfiguration staticConfig {
    static_cast<CharIOStreamIf&>(ioStream),
    std::vector<User>{{"admin", "admin123", AccessLevel::Admin}},
    staticTree.getRoot(),  // Pass reference instead of pointer
    inputTimeout_ms,
    commandHistorySize
  };

  // OR using mixed allocation
  auto mixedTree = createMixedMenuTree();
  
  CLIServiceConfiguration mixedConfig {
    static_cast<CharIOStreamIf&>(ioStream),
    std::vector<User>{{"admin", "admin123", AccessLevel::Admin}},
    std::move(mixedTree),  // This constructor takes unique_ptr
    inputTimeout_ms,
    commandHistorySize
  };

  // Create service with static configuration
  CLIService cli(std::move(staticConfig));
  
  cli.activate();

  while (cli.getState() != CLIState::Inactive)
  {
    cli.service();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return 0;
}
