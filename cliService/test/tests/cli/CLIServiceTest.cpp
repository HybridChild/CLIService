#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "cliService/cli/CLIService.hpp"
#include "mock/tree/CommandMock.hpp"
#include "mock/terminal/TerminalMock.hpp"

using namespace cliService;

class CLIServiceTest : public ::testing::Test 
{
protected:
  void SetUp() override 
  {
    auto root = std::make_unique<Directory>("root", AccessLevel::User);
    _rootDir = root.get();
    
    auto& subDir = _rootDir->addDirectory("sub", AccessLevel::User);
    _mockCmd = &subDir.addCommand<CommandMock>("test", AccessLevel::Admin);
    
    std::vector<User> users = {
      {"admin", "admin123", AccessLevel::Admin},
      {"user", "pass123", AccessLevel::User}
    };
    
    _service = std::make_unique<CLIService>(
      CLIServiceConfiguration{
        _terminal, std::move(users), std::move(root)
      });
  }

  TerminalMock _terminal;
  Directory* _rootDir;
  CommandMock* _mockCmd;
  std::unique_ptr<CLIService> _service;
};

TEST_F(CLIServiceTest, LoginSuccess) 
{
  _terminal.queueInput("admin:admin123\r");
  
  _service->activate();
  _service->service();
  
  EXPECT_THAT(_terminal.getOutput(), testing::HasSubstr("admin@> "));
}

TEST_F(CLIServiceTest, ExecuteCommand) 
{
 _terminal.queueInput("admin:admin123\r");  // Login first
 _terminal.queueInput("sub/test arg1 arg2\r");
 
 EXPECT_CALL(*_mockCmd, execute(testing::ElementsAre("arg1", "arg2")));
 
 _service->activate();
 _service->service();  // Handle login
 _service->service();  // Handle command
}

TEST_F(CLIServiceTest, AccessDenied) 
{
 _terminal.queueInput("user:pass123\r");  // Login as regular user
 _terminal.queueInput("sub/test arg1\r"); // Try to access admin command
 
 EXPECT_CALL(*_mockCmd, execute).Times(0);
 
 _service->activate();
 _service->service();
 _service->service();
 
 EXPECT_THAT(_terminal.getOutput(), testing::HasSubstr("Access denied"));
}

TEST_F(CLIServiceTest, GlobalCommandLogout) 
{
 _terminal.queueInput("admin:admin123\r");
 _terminal.queueInput("logout\r");
 
 _service->activate();
 _service->service();
 _service->service();
 
 EXPECT_THAT(_terminal.getOutput(), testing::HasSubstr("Logged out"));
}

TEST_F(CLIServiceTest, InvalidPath) 
{
 _terminal.queueInput("admin:admin123\r");
 _terminal.queueInput("invalid/path\r");
 
 _service->activate();
 _service->service();
 _service->service();
 
 EXPECT_THAT(_terminal.getOutput(), testing::HasSubstr("Invalid path"));
}

TEST_F(CLIServiceTest, NavigateToDirectory) 
{
 _terminal.queueInput("admin:admin123\r");
 _terminal.queueInput("sub\r");
 
 _service->activate();
 _service->service();
 _service->service();
 
 EXPECT_THAT(_terminal.getOutput(), testing::EndsWith("admin@> "));
}

TEST_F(CLIServiceTest, ExitFromAnyState) 
{
 _terminal.queueInput("exit\r");
 
 _service->activate();
 _service->service();
 
 EXPECT_THAT(_terminal.getOutput(), testing::HasSubstr("inactive"));
}
