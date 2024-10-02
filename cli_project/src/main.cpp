#include "cli/CLIService.hpp"
#include "cli/CLIServiceConfiguration.hpp"
#include "io/InOutStream.hpp"
#include "menu/CommandMenuTreeFactory.hpp"

int main() {
  auto inOutStream = std::make_unique<InOutStream>();
  auto tree = CommandMenuTreeFactory::createDefaultTree();
  auto config = std::make_unique<CLIServiceConfiguration>(std::move(tree), std::move(inOutStream));

  CLIService cli(std::move(config));
  cli.activate();

  while (true) {
    cli.service();
    if (!cli.isRunning()) {
      inOutStream->write("Thank you for using the CLI Service. Goodbye!\n");
      break;
    }
  }
  
  return 0;
}
