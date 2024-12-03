#include "cliService/tree/CommandIf.hpp"

namespace cliService
{

  CommandIf::CommandIf(std::string name, AccessLevel level)
    : NodeIf(std::move(name), level)
  {}

  bool CommandIf::isDirectory() const { return false; }

}
