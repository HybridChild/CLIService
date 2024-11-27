#include <unordered_map>
#include "cli/CLIService.hpp"
#include "factory/CommandMenuTreeFactory.hpp"
#include "user/User.hpp"
#include "menu/Command.hpp"
#include "commands/accessLevel.hpp"
#include "io/StdIOStream.hpp"


int main() {
  auto ioStream = std::make_unique<StdIOStream>();
  auto tree = cliService::CommandMenuTreeFactory::createDefaultTree();

  std::unordered_map<std::string, cliService::User> users = {
    {"admin"    , cliService::User("admin"    , "admin123", cliService::AccessLevel::Admin)},
    {"poweruser", cliService::User("poweruser", "power123", cliService::AccessLevel::Advanced)},
    {"user"     , cliService::User("user"     , "user123" , cliService::AccessLevel::Basic)}
  };

  auto config = std::make_unique<cliService::CLIServiceConfiguration>(
    std::move(tree),
    std::move(ioStream),
    std::move(users)
  );

  cliService::CLIService cli(std::move(config));
  cli.activate();

  while (cli.isRunning()) {
    cli.service();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  
  return 0;
}
