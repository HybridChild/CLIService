#include "cli/CLIService.hpp"
#include "cli/CLIServiceConfiguration.hpp"
#include "io/InOutStream.hpp"
#include "menu/CommandMenuTreeFactory.hpp"
#include <unordered_map>

int main() {
  auto inOutStream = std::make_unique<InOutStream>();
  auto tree = CommandMenuTreeFactory::createDefaultTree();

  std::unordered_map<std::string, std::string> users = {
    {"admin", "admin123"},
    {"user", "user123"}
  };

  auto config = std::make_unique<CLIServiceConfiguration>(
    std::move(tree),
    std::move(inOutStream),
    std::move(users)
  );

  CLIService cli(std::move(config));
  cli.activate();

  while (true) {
    cli.service();
    if (!cli.isRunning()) {
      break;
    }
  }
  
  return 0;
}
