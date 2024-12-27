#pragma once
#include <memory>
#include "cliService/tree/Directory.hpp"
#include "commands/AccessLevel.hpp"
#include "commands/system/RebootCommand.hpp"
#include "commands/system/HeapStatsGetCommand.hpp"
#include "commands/hw/PotmeterGetCommand.hpp"
#include "commands/hw/RgbLedSetCommand.hpp"
#include "commands/hw/ToggleSwitchGetCommand.hpp"

using namespace cliService;

// Example of mixed allocation
std::unique_ptr<Directory> createMixedMenuTree()
{
  // Create dynamic root
  auto dirRoot = std::make_unique<Directory>("root", AccessLevel::User);
    
  // Add static system directory
  static Directory sysDir("system", AccessLevel::Admin);
  dirRoot->addStaticDirectory(sysDir);
    
  // Add dynamic command to static directory
  static RebootCommand rebootCmd("reboot", AccessLevel::Admin);
  sysDir.addStaticCommand(rebootCmd);

  // Add static command to static directory
  sysDir.addDynamicCommand<HeapStatsGetCommand>("heap", AccessLevel::Admin);
    
  // Add dynamic hardware directory with dynamic command
  auto& hwDir = dirRoot->addDynamicDirectory("hw", AccessLevel::User);
  hwDir.addDynamicCommand<RgbLedSetCommand>("setRgb", AccessLevel::Admin);
    
  return dirRoot;
}
