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


std::unique_ptr<Directory> createMenuTree()
{
  auto dirRoot = std::make_unique<cliService::Directory>("root", AccessLevel::User);
    
    auto& dirSystem = dirRoot->addDirectory("system", AccessLevel::Admin);
      dirSystem.addCommand<RebootCommand>("reboot", AccessLevel::Admin);
      dirSystem.addCommand<HeapStatsGetCommand>("heap", AccessLevel::Admin);

    auto& dirHw = dirRoot->addDirectory("hw", AccessLevel::User);
      auto& dirHwPot = dirHw.addDirectory("potmeter", AccessLevel::User);
        dirHwPot.addCommand<PotmeterGetCommand>("get", AccessLevel::User);

      auto& dirHwRgbLed = dirHw.addDirectory("rgbLed", AccessLevel::Admin);
        dirHwRgbLed.addCommand<RgbLedSetCommand>("set", AccessLevel::Admin);

      auto& dirHwToggleSwitch = dirHw.addDirectory("toggleSwitch", AccessLevel::User);
        dirHwToggleSwitch.addCommand<ToggleSwitchGetCommand>("get", AccessLevel::User);

  return std::move(dirRoot);
}


int main()
{
  UnixWinCharIOStream ioStream{};

  std::vector<cliService::User> users = {
    {"admin", "admin123", AccessLevel::Admin},
    {"user", "user123", AccessLevel::User}
  };

  auto tree = createMenuTree();

  CLIServiceConfiguration cliConfig {
    static_cast<CharIOStreamIf&>(ioStream),
    std::move(users),
    std::move(tree),
    inputTimeout_ms,
    commandHistorySize
  };
  
  CLIService cli(std::move(cliConfig));
  
  cli.activate();

  while (cli.getState() != CLIState::Inactive)
  {
    cli.service();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return 0;
}
