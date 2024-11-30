#include "cliService/tree/CommandIf.hpp"

namespace cliService
{

  CommandIf::CommandIf(std::string name)
    : NodeIf(std::move(name))
  {}

  bool CommandIf::isDirectory() const { return false; }

}
