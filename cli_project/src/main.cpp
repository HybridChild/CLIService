#include "cli/CLIService.hpp"
#include "menu/CommandMenuTreeFactory.hpp"

int main() {
  auto tree = CommandMenuTreeFactory::createDefaultTree();
  CLIService cli(std::move(tree));
  cli.run();
  return 0;
}
