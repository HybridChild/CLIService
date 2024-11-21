#include "cli/CLIService.hpp"
#include "menu/CommandMenuTreeFactory.hpp"
#include "io/StdIOStream.hpp"
#include "user/User.hpp"
#include "command/Command.hpp"
#include <unordered_map>


int main() {
  auto inOutStream = std::make_unique<StdIOStream>();
  auto tree = CommandMenuTreeFactory::createDefaultTree();

  std::unordered_map<std::string, User> users = {
    {"admin"    , User("admin"    , "admin123", Command::AccessLevel::Admin)},
    {"poweruser", User("poweruser", "power123", Command::AccessLevel::Advanced)},
    {"user"     , User("user"     , "user123" , Command::AccessLevel::Basic)}
  };

  auto config = std::make_unique<CLIServiceConfiguration>(
    std::move(tree),
    std::move(inOutStream),
    std::move(users)
  );

  CLIService cli(std::move(config));
  cli.activate();

  while (cli.isRunning()) {
    cli.service();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  
  return 0;
}
