#pragma once
#include "commands/AccessLevel.hpp"
#include "commands/system/RebootCommand.hpp"
#include "commands/system/HeapStatsGetCommand.hpp"
#include "commands/hw/RgbLedSetCommand.hpp"
#include "commands/hw/PotmeterGetCommand.hpp"
#include "commands/hw/ToggleSwitchGetCommand.hpp"

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
