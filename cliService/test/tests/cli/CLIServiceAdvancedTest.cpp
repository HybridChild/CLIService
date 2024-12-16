#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "cliService/cli/CLIService.hpp"
#include "mock/command/CommandMock.hpp"
#include "mock/io/CharIOStreamMock.hpp"

namespace cliService
{


  class CLIServiceAdvancedTest : public ::testing::Test 
  {
    static constexpr size_t HISTORY_SIZE = 10;
    
  protected:
    void SetUp() override 
    {
      auto root = std::make_unique<Directory>("root", AccessLevel::User);
      _rootDir = root.get();

      // Create a more complex directory structure
      auto& adminDir = _rootDir->addDirectory("admin", AccessLevel::Admin);
      _adminCmd = &adminDir.addCommand<CommandMock>("config", AccessLevel::Admin);

      auto& publicDir = _rootDir->addDirectory("public", AccessLevel::User);
      _publicCmd = &publicDir.addCommand<CommandMock>("info", AccessLevel::User);

      auto& nestedDir = publicDir.addDirectory("nested", AccessLevel::User);
      _nestedCmd = &nestedDir.addCommand<CommandMock>("test", AccessLevel::User);

      std::vector<User> users = {
        {"admin", "admin123", AccessLevel::Admin},
        {"user", "user123", AccessLevel::User}
      };

      _service = std::make_unique<CLIService>(CLIServiceConfiguration{_ioStream, std::move(users), std::move(root), HISTORY_SIZE});
    }

    CharIOStreamMock _ioStream;
    Directory* _rootDir;
    CommandMock* _adminCmd;
    CommandMock* _publicCmd;
    CommandMock* _nestedCmd;
    std::unique_ptr<CLIService> _service;
  };

  TEST_F(CLIServiceAdvancedTest, ComplexNavigation)
  {
    _ioStream.queueInput("admin:admin123\n");  // Login as admin
    _service->activate();
    _service->service();

    // First verify we can go into nested directory
    _ioStream.queueInput("public/nested\n");   
    _service->service();
    EXPECT_THAT(_ioStream.getOutput(), testing::EndsWith("admin@/public/nested > "));

    // From /public/nested, go back to root and then to admin
    _ioStream.queueInput("../../admin\n");  // Need three levels to account for being at root
    _service->service();

    EXPECT_THAT(_ioStream.getOutput(), testing::EndsWith("admin@/admin > "));
  }

  TEST_F(CLIServiceAdvancedTest, AccessLevelStateTransitions)
  {
    // Login as regular user
    _ioStream.queueInput("user:user123\n");
    _service->activate();
    _service->service();

    // Try accessing admin directory
    _ioStream.queueInput("admin/config\n");
    _service->service();
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("Access denied"));

    // Logout
    _ioStream.queueInput("logout\n");
    _service->service();
    EXPECT_EQ(_service->getState(), CLIState::LoggedOut);

    // Login as admin
    _ioStream.queueInput("admin:admin123\n");
    _service->service();

    // Set expectation before queueing the command
    EXPECT_CALL(*_adminCmd, execute(testing::_))
      .WillOnce(testing::Return(CommandResponse::success()));

    // Now try accessing admin directory
    _ioStream.queueInput("admin/config\n");
    _service->service();
  }

  TEST_F(CLIServiceAdvancedTest, ErrorHandling)
  {
    _ioStream.queueInput("admin:admin123\n");
    _service->activate();
    _service->service();

    // Test various error cases
    _ioStream.queueInput("nonexistent/path\n");
    _service->service();
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("Invalid path"));

    _ioStream.queueInput("public/info invalid args\n");
    EXPECT_CALL(*_publicCmd, execute(testing::_))
      .WillOnce(testing::Return(CommandResponse("Invalid arguments", CommandStatus::InvalidArguments)));
    _service->service();
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("Invalid arguments"));
  }

  TEST_F(CLIServiceAdvancedTest, GlobalCommands)
  {
    _ioStream.queueInput("admin:admin123\n");
    _service->activate();
    _service->service();

    // Test tree command
    _ioStream.queueInput("tree\n");
    _service->service();
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("admin/"));
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("public/"));

    // Test logout from nested location
    _ioStream.queueInput("public/nested\n");
    _ioStream.queueInput("logout\n");
    _service->service();
    _service->service();

    EXPECT_EQ(_service->getState(), CLIState::LoggedOut);
    // Should reset to root after logout
    _ioStream.queueInput("admin:admin123\n");
    _service->service();
    EXPECT_THAT(_ioStream.getOutput(), testing::EndsWith("admin@/ > "));
  }

  TEST_F(CLIServiceAdvancedTest, GlobalCommandExit) 
  {
    _ioStream.queueInput("admin:admin123\n");
    _service->activate();
    _service->service();
    EXPECT_EQ(_service->getState(), CLIState::LoggedIn);

    _ioStream.clearOutput();

    _ioStream.queueInput("exit\n");
    _service->service();

    EXPECT_EQ(_service->getState(), CLIState::Inactive);
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("Exiting CLI Service."));

    // Verify service no longer processes input after exit
    _ioStream.clearOutput();
    _ioStream.queueInput("command\n");
    _service->service();
    EXPECT_TRUE(_ioStream.getOutput().empty());
  }

  // Test exit command with arguments (should fail)
  TEST_F(CLIServiceAdvancedTest, GlobalCommandExitWithArgs) 
  {
    _ioStream.queueInput("admin:admin123\n");
    _service->activate();
    _service->service();

    _ioStream.clearOutput();

    // Try exit with arguments
    _ioStream.queueInput("exit arg1\n");
    _service->service();

    // Verify still in logged in state
    EXPECT_EQ(_service->getState(), CLIState::LoggedIn);
    EXPECT_THAT(_ioStream.getOutput(), testing::HasSubstr("Command takes no arguments."));
  }

  TEST_F(CLIServiceAdvancedTest, PathResolutionEdgeCases)
  {
    _ioStream.queueInput("admin:admin123\n");  // Login as admin
    _service->activate();
    _service->service();

    // Test going up beyond root
    _ioStream.queueInput("../../../../\n");
    _service->service();
    EXPECT_THAT(_ioStream.getOutput(), testing::EndsWith("admin@/ > "));

    // Test normalized paths
    EXPECT_CALL(*_nestedCmd, execute(testing::_))
      .WillOnce(testing::Return(CommandResponse::success()));

    _ioStream.queueInput("/public/nested/./test\n");  // Using . in path
    _service->service();

    // Verify double slash handling
    _ioStream.clearOutput();
    EXPECT_CALL(*_nestedCmd, execute(testing::_))
      .WillOnce(testing::Return(CommandResponse::success()));

    _ioStream.queueInput("public//nested//test\n");  // Double slashes
    _service->service();
  }

  TEST_F(CLIServiceAdvancedTest, TreeCommandAccessFiltering)
  {
    // First test as regular user
    _ioStream.queueInput("user:user123\n");
    _service->activate();
    _service->service();

    _ioStream.clearOutput();
    _ioStream.queueInput("tree\n");
    _service->service();

    // Check user view - should see public stuff but not admin
    std::string userOutput = _ioStream.getOutput();
    EXPECT_THAT(userOutput, testing::HasSubstr("public/"));
    EXPECT_THAT(userOutput, testing::HasSubstr("nested/"));
    EXPECT_THAT(userOutput, testing::Not(testing::HasSubstr("admin/")));

    // Logout
    _ioStream.queueInput("logout\n");
    _service->service();

    // Now test as admin
    _ioStream.queueInput("admin:admin123\n");
    _service->service();

    _ioStream.clearOutput();
    _ioStream.queueInput("tree\n");
    _service->service();

    // Check admin view - should see everything
    std::string adminOutput = _ioStream.getOutput();
    EXPECT_THAT(adminOutput, testing::HasSubstr("public/"));
    EXPECT_THAT(adminOutput, testing::HasSubstr("nested/"));
    EXPECT_THAT(adminOutput, testing::HasSubstr("admin/"));
    EXPECT_THAT(adminOutput, testing::HasSubstr("config"));
  }

}
