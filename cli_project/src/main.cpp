#include "cli/CLIService.hpp"
#include "cli/CLIServiceConfiguration.hpp"
#include "stream/InOutStream.hpp"
#include "menu/CommandMenuTreeFactory.hpp"

int main() {
  auto inOutStream = std::make_unique<InOutStream>();
  auto tree = CommandMenuTreeFactory::createDefaultTree();
  auto config = std::make_unique<CLIServiceConfiguration>(std::move(tree), std::move(inOutStream));

  CLIService cli(std::move(config));

  cli.run();
  
  return 0;
}
