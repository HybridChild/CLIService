#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "cliService/cli/CLIService.hpp"
#include "mock/command/CommandMock.hpp"
#include "mock/io/CharIOStreamMock.hpp"

namespace cliService
{

  class CLIServiceTest : public ::testing::Test
  {
    static constexpr size_t HISTORY_SIZE = 10;

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

      _service = std::make_unique<CLIService>(CLIServiceConfiguration{_ioStream, std::move(users), std::move(root), HISTORY_SIZE});
    }

    CharIOStreamMock _ioStream;
    Directory* _rootDir;
    CommandMock* _mockCmd;
    std::unique_ptr<CLIService> _service;
  };

  TEST_F(CLIServiceTest, StartsInLoggedOutState) 
  {
    _service->activate();
    EXPECT_EQ(_service->getState(), CLIState::LoggedOut);
  }

  TEST_F(CLIServiceTest, LoginSuccess) 
  {
    _ioStream.queueInput("admin:admin123\n");

    _service->activate();
    _service->service();

    EXPECT_EQ(_service->getState(), CLIState::LoggedIn);
  }

  TEST_F(CLIServiceTest, ExecuteCommand) 
  {
    _ioStream.queueInput("admin:admin123\n");  // Login first
    _ioStream.queueInput("sub/test arg1 arg2\n");

    EXPECT_CALL(*_mockCmd, execute(testing::ElementsAre("arg1", "arg2")))
      .WillOnce(testing::Return(CommandResponse::success()));

    _service->activate();
    _service->service();  // Handle login
    _service->service();  // Handle command
  }

  TEST_F(CLIServiceTest, AccessDenied) 
  {
    _ioStream.queueInput("user:pass123\n");  // Login as regular user
    _ioStream.queueInput("sub/test arg1\n"); // Try to access admin command

    EXPECT_CALL(*_mockCmd, execute).Times(0);

    _service->activate();
    _service->service();
    _service->service();

    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("Access denied"));
  }

  TEST_F(CLIServiceTest, GlobalCommandLogout) 
  {
    _ioStream.queueInput("admin:admin123\n");
    _ioStream.queueInput("logout\n");

    _service->activate();
    _service->service();
    _service->service();

    EXPECT_EQ(_service->getState(), CLIState::LoggedOut);
  }

  TEST_F(CLIServiceTest, InvalidPath) 
  {
    _ioStream.queueInput("admin:admin123\n");
    _ioStream.queueInput("invalid/path\n");

    _service->activate();
    _service->service();
    _service->service();

    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("Invalid path"));
  }

  TEST_F(CLIServiceTest, NavigateToDirectory) 
  {
    _ioStream.queueInput("admin:admin123\n");
    _ioStream.queueInput("sub\n");

    _service->activate();
    _service->service();
    _service->service();

    EXPECT_THAT(_ioStream.getOutput(), testing::EndsWith("admin@/sub > "));
  }

  TEST_F(CLIServiceTest, InvalidLoginFormat)
  {
    _ioStream.queueInput("invalidloginformat\n");
    _service->activate();
    _service->service();
    
    EXPECT_THAT(_ioStream.getOutput(), 
      testing::HasSubstr(CLIService::getLoggedOutMessage()));
    EXPECT_EQ(_service->getState(), CLIState::LoggedOut);
  }

  TEST_F(CLIServiceTest, EmptyUsername)
  {
    _ioStream.queueInput(":password123\n");
    _service->activate();
    _service->service();
    
    EXPECT_THAT(_ioStream.getOutput(), 
      testing::HasSubstr(CLIService::getLoggedOutMessage()));
    EXPECT_EQ(_service->getState(), CLIState::LoggedOut);
  }

  TEST_F(CLIServiceTest, EmptyPassword)
  {
    _ioStream.queueInput("username:\n");
    _service->activate();
    _service->service();
    
    EXPECT_THAT(_ioStream.getOutput(), 
      testing::HasSubstr(CLIService::getLoggedOutMessage()));
    EXPECT_EQ(_service->getState(), CLIState::LoggedOut);
  }

}
