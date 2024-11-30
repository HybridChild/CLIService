#include "cliService/tree/Command.hpp"

namespace cliService
{

  Command::Command(std::string name)
    : Node(std::move(name))
  {}

  bool Command::isDirectory() const { return false; }

}
