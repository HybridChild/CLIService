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
    static constexpr uint32_t INPUT_TIMEOUT_MS = 1000;

  protected:
    void SetUp() override
    {
      // Create a complex directory structure for comprehensive testing
      auto root = std::make_unique<Directory>("root", AccessLevel::User);
      _rootDir = root.get();

      // Admin directory with restricted access
      auto& adminDir = _rootDir->addDynamicDirectory("admin", AccessLevel::Admin);
      _adminCmd = &adminDir.addDynamicCommand<CommandMock>("config", AccessLevel::Admin, "Admin configuration");

      // Public directory structure
      auto& publicDir = _rootDir->addDynamicDirectory("public", AccessLevel::User);
      _publicCmd = &publicDir.addDynamicCommand<CommandMock>("info", AccessLevel::User, "Public information");

      auto& nestedDir = publicDir.addDynamicDirectory("nested", AccessLevel::User);
      _nestedCmd = &nestedDir.addDynamicCommand<CommandMock>("test", AccessLevel::User, "Nested test command");

      // Users setup
      std::vector<User> users = {
        {"admin", "admin123", AccessLevel::Admin},
        {"user", "user123", AccessLevel::User}
      };

      // Custom messages for testing output
      auto messages = CLIMessages::getDefaults();
      messages.setWelcomeMessage("Welcome Test");
      messages.setLoggedInMessage("Login Success");
      messages.setLoggedOutMessage("Logout Success");
      messages.setExitMessage("Exit Success");
      messages.setAccessDeniedMessage("Access Denied Test");
      messages.setInvalidPathMessage("Invalid Path Test");
      messages.setInvalidLoginMessage("Invalid Login Test");

      CLIServiceConfiguration config{
        _ioStream,
        std::move(users),
        std::move(root),
        INPUT_TIMEOUT_MS,
        HISTORY_SIZE,
        std::move(messages)
      };

      _service = std::make_unique<CLIService>(std::move(config));
    }

    CharIOStreamMock _ioStream;
    Directory* _rootDir;
    CommandMock* _adminCmd;
    CommandMock* _publicCmd;
    CommandMock* _nestedCmd;
    std::unique_ptr<CLIService> _service;
  };

  // Basic State Tests

  TEST_F(CLIServiceTest, InitialState)
  {
    EXPECT_EQ(_service->getCLIState(), CLIState::Inactive);
    _service->activate();
    EXPECT_EQ(_service->getCLIState(), CLIState::LoggedOut);
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("Welcome Test"));
  }

  // Authentication Tests

  TEST_F(CLIServiceTest, ValidLogin)
  {
    _service->activate();
    _ioStream.queueInput("admin:admin123\n");
    _service->service();
    
    EXPECT_EQ(_service->getCLIState(), CLIState::LoggedIn);
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("Login Success"));
    EXPECT_THAT(_ioStream.getOutput(), testing::EndsWith("admin@/> "));
  }

  TEST_F(CLIServiceTest, InvalidLoginAttempts)
  {
    _service->activate();
    
    // Test various invalid login formats
    std::vector<std::string> invalidLogins = {
      "invalid\n",           // No password separator
      ":password123\n",      // Empty username
      "username:\n",         // Empty password
      "wrong:password\n"     // Invalid credentials
    };

    for (const auto& login : invalidLogins) {
      _ioStream.clearOutput();
      _ioStream.queueInput(login);
      _service->service();
      
      EXPECT_EQ(_service->getCLIState(), CLIState::LoggedOut);
      EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("Invalid Login Test"));
    }
  }

  // Command Execution Tests

  TEST_F(CLIServiceTest, BasicCommandExecution)
  {
    _service->activate();
    _ioStream.queueInput("admin:admin123\n");
    _service->service();

    EXPECT_CALL(*_publicCmd, execute(testing::ElementsAre("arg1", "arg2")))
      .WillOnce(testing::Return(Response::success(std::string("Command executed"))));

    _ioStream.queueInput("public/info arg1 arg2\n");
    _service->service();

    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("Command executed"));
  }

  TEST_F(CLIServiceTest, AccessControlEnforcement)
  {
    // Login as regular user
    _service->activate();
    _ioStream.queueInput("user:user123\n");
    _service->service();

    // Try to access admin command
    _ioStream.clearOutput();
    _ioStream.queueInput("admin/config\n");
    _service->service();

    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("Access Denied Test"));
  }

  // Navigation Tests

  TEST_F(CLIServiceTest, DirectoryNavigation)
  {
    _service->activate();
    _ioStream.queueInput("admin:admin123\n");
    _service->service();

    // Test basic navigation
    _ioStream.queueInput("public/nested\n");
    _service->service();
    EXPECT_THAT(_ioStream.getOutput(), testing::EndsWith("admin@/public/nested> "));

    // Test parent directory navigation
    _ioStream.queueInput("../../\n");
    _service->service();
    EXPECT_THAT(_ioStream.getOutput(), testing::EndsWith("admin@/> "));
  }

  // Path Resolution Tests

  TEST_F(CLIServiceTest, PathResolutionEdgeCases)
  {
    _service->activate();
    _ioStream.queueInput("admin:admin123\n");
    _service->service();

    // Test going beyond root - should stay at root
    _ioStream.clearOutput();
    _ioStream.queueInput("../../../../\n");
    _service->service();
    EXPECT_THAT(_ioStream.getOutput(), testing::EndsWith("admin@/> "));

    // Test path with dot - should execute test command
    EXPECT_CALL(*_nestedCmd, execute(testing::ElementsAre()))
      .WillOnce(testing::Return(Response::success()));
    _ioStream.clearOutput();
    _ioStream.queueInput("/public/nested/./test\n");
    _service->service();

    // Test double slashes - should also execute test command
    EXPECT_CALL(*_nestedCmd, execute(testing::ElementsAre()))
      .WillOnce(testing::Return(Response::success()));
    _ioStream.clearOutput();
    _ioStream.queueInput("public//nested//test\n");
    _service->service();

    // Test invalid path
    _ioStream.clearOutput();
    _ioStream.queueInput("nonexistent/path\n");
    _service->service();
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("Invalid Path Test"));
  }

  // Global Command Tests

  TEST_F(CLIServiceTest, GlobalCommands)
  {
    _service->activate();
    _ioStream.queueInput("admin:admin123\n");
    _service->service();

    // Test help command
    _ioStream.clearOutput();
    _ioStream.queueInput("help\n");
    _service->service();
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("help   -"));
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("tree   -"));

    // Test tree command
    _ioStream.clearOutput();
    _ioStream.queueInput("tree\n");
    _service->service();
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("admin/"));
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("public/"));

    // Test logout command
    _ioStream.clearOutput();
    _ioStream.queueInput("logout\n");
    _service->service();
    EXPECT_EQ(_service->getCLIState(), CLIState::LoggedOut);
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("Logout Success"));

    // Test exit command
    _ioStream.queueInput("admin:admin123\n");
    _service->service();
    _ioStream.clearOutput();
    _ioStream.queueInput("exit\n");
    _service->service();
    EXPECT_EQ(_service->getCLIState(), CLIState::Inactive);
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("Exit Success"));
  }

  TEST_F(CLIServiceTest, GlobalCommandArgumentValidation)
  {
    _service->activate();
    _ioStream.queueInput("admin:admin123\n");
    _service->service();

    std::vector<std::string> commands = {
      "help arg\n",
      "tree arg\n",
      "logout arg\n",
      "exit arg\n"
    };

    for (const auto& cmd : commands) {
      _ioStream.clearOutput();
      _ioStream.queueInput(cmd);
      _service->service();
      EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("Command takes no arguments"));
    }
  }

  // Output Formatting Tests

  TEST_F(CLIServiceTest, OutputFormatting)
  {
    _service->activate();
    _ioStream.queueInput("admin:admin123\n");
    _service->service();

    // Test command with multiple lines of output
    EXPECT_CALL(*_publicCmd, execute(testing::_))
      .WillOnce(testing::Return(Response(std::string("Line 1\nLine 2\nLine 3"), ResponseStatus::Success)));

    _ioStream.clearOutput();
    _ioStream.queueInput("public/info\n");
    _service->service();

    std::string output = _ioStream.getOutput();
    EXPECT_THAT(output, testing::HasSubstr("Line 1"));
    EXPECT_THAT(output, testing::HasSubstr("Line 2"));
    EXPECT_THAT(output, testing::HasSubstr("Line 3"));
  }

  // Access Level Output Tests

  TEST_F(CLIServiceTest, AccessLevelOutput)
  {
    _service->activate();
    
    // Test as regular user
    _ioStream.queueInput("user:user123\n");
    _service->service();
    _ioStream.clearOutput();
    _ioStream.queueInput("tree\n");
    _service->service();
    
    std::string userOutput = _ioStream.getOutput();
    EXPECT_THAT(userOutput, testing::HasSubstr("public/"));
    EXPECT_THAT(userOutput, testing::Not(testing::HasSubstr("admin/")));

    // Test as admin
    _ioStream.queueInput("logout\n");
    _service->service();
    _ioStream.queueInput("admin:admin123\n");
    _service->service();
    _ioStream.clearOutput();
    _ioStream.queueInput("tree\n");
    _service->service();
    
    std::string adminOutput = _ioStream.getOutput();
    EXPECT_THAT(adminOutput, testing::HasSubstr("public/"));
    EXPECT_THAT(adminOutput, testing::HasSubstr("admin/"));
  }

  TEST_F(CLIServiceTest, CommandHistoryBasic)
  {
    _service->activate();
    _ioStream.queueInput("admin:admin123\n");
    _service->service();
    _ioStream.clearOutput();

    EXPECT_CALL(*_publicCmd, execute(testing::ElementsAre("first")))
      .WillOnce(testing::Return(Response::success(std::string("Command executed"))));

    // Execute first command
    _ioStream.queueInput("public/info first\n");
    _service->service();

    EXPECT_CALL(*_publicCmd, execute(testing::ElementsAre("second")))
      .WillOnce(testing::Return(Response::success(std::string("Command executed"))));
    
    // Execute second command
    _ioStream.queueInput("public/info second\n");
    _service->service();
    _ioStream.clearOutput();

    // Navigate up
    _ioStream.queueInput({0x1B, '[', 'A'}); // Up arrow
    _service->service();
    
    // Should show most recent command
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("public/info second"));
  }

  TEST_F(CLIServiceTest, CommandHistoryWithPartialInput)
  {
    _service->activate();
    _ioStream.queueInput("admin:admin123\n");
    _service->service();

    EXPECT_CALL(*_publicCmd, execute(testing::ElementsAre("test")))
      .WillOnce(testing::Return(Response::success(std::string("Command executed"))));

    // Add command to history
    _ioStream.queueInput("public/info test\n");
    _service->service();
    _ioStream.clearOutput();

    // Start typing new command
    _ioStream.queueInput("pub");
    _service->service();
    _ioStream.clearOutput();

    // Navigate up - should save current input
    _ioStream.queueInput({0x1B, '[', 'A'});
    _service->service();
    
    // Should show history and clear current input
    std::string output = _ioStream.getOutput();
    EXPECT_THAT(output, testing::HasSubstr("\b \b\b \b\b \b")); // Clear "pub"
    EXPECT_THAT(output, testing::HasSubstr("public/info test")); // Show history

    // Navigate down - should restore original input
    _ioStream.clearOutput();
    _ioStream.queueInput({0x1B, '[', 'B'});
    _service->service();
    
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("pub"));
  }

  TEST_F(CLIServiceTest, CommandHistoryEdgeCases)
  {
    _service->activate();
    _ioStream.queueInput("admin:admin123\n");
    _service->service();
    _ioStream.clearOutput();

    // Try navigating history when empty
    _ioStream.queueInput({0x1B, '[', 'A'});
    _service->service();
    EXPECT_EQ(_ioStream.getOutput(), "");

    EXPECT_CALL(*_publicCmd, execute(testing::ElementsAre("test")))
      .WillOnce(testing::Return(Response::success(std::string("Command executed"))));

    // Navigate past history boundaries
    _ioStream.queueInput("public/info test\n");
    _service->service();
    _ioStream.clearOutput();

    // Up arrow multiple times
    for (int i = 0; i < 3; i++) {
        _ioStream.queueInput({0x1B, '[', 'A'});
        _service->service();
    }

    // Should stay at oldest command
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("public/info test"));

    // Down arrow past newest
    _ioStream.queueInput({0x1B, '[', 'B'});
    _ioStream.queueInput({0x1B, '[', 'B'});
    _service->service();

    // Should clear input
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("\b \b"));
  }

}
