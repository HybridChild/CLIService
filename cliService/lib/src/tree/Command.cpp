#include "cliService/tree/Command.hpp"

namespace cliService
{

  Command::Command(std::string name)
    : NodeIf(std::move(name))
  {}

  bool Command::isDirectory() const { return false; }

}
