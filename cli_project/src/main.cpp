#include "cli/CLIService.hpp"
#include "menu/CommandMenuTreeFactory.hpp"
#include "io/InOutStream.hpp"
#include "user/User.hpp"
#include "command/Command.hpp"
#include <unordered_map>


int main() {
  auto inOutStream = std::make_unique<InOutStream>();
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
  }
  
  return 0;
}
