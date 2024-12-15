#include "gtest/gtest.h"
#include "cliService/cli/CLIService.hpp"
#include "mock/command/CommandMock.hpp"
#include "mock/terminal/TerminalMock.hpp"

namespace cliService
{

  class TabCompletionTest : public ::testing::Test
  {
    static constexpr size_t HISTORY_SIZE = 10;

  protected:
    void SetUp() override
    {
      // Create a test directory structure:
      // /
      // ├── folder1/
      // │   ├── subfolder1/
      // │   │   └── deep/
      // │   └── subfolder2/
      // └── folder2/
      //     ├── target1/
      //     └── target2/

      auto root = std::make_unique<Directory>("root", AccessLevel::User);

      auto &folder1 = root->addDirectory("folder1", AccessLevel::User);
      auto &subfolder1 = folder1.addDirectory("subfolder1", AccessLevel::User);
      subfolder1.addDirectory("deep", AccessLevel::User);
      folder1.addDirectory("subfolder2", AccessLevel::User);

      auto &folder2 = root->addDirectory("folder2", AccessLevel::User);
      folder2.addDirectory("target1", AccessLevel::User);
      folder2.addDirectory("target2", AccessLevel::User);

      std::vector<User> users = {
          {"admin", "admin123", AccessLevel::Admin},
          {"user", "user123", AccessLevel::User}};

      _service = std::make_unique<CLIService>(
          CLIServiceConfiguration{_terminal, std::move(users), std::move(root), HISTORY_SIZE});
    }

    void loginAsUser()
    {
      _terminal.queueInput("user:user123\n");
      _service->activate();
      _service->service();
      _terminal.clearOutput();
    }

    void navigateToSubfolder1()
    {
      _terminal.queueInput("folder1/subfolder1\n");
      _service->service();
      _terminal.clearOutput();
    }

    void pressTab()
    {
      _terminal.queueInput("\t");
      _service->service();
    }

    TerminalMock _terminal;
    std::unique_ptr<CLIService> _service;
  };

  TEST_F(TabCompletionTest, TabCompleteParentDirectory)
  {
    loginAsUser();
    navigateToSubfolder1(); // Now in /folder1/subfolder1

    _terminal.queueInput("../../f");
    _service->service();
    pressTab();

    std::string output = _terminal.getOutput();
    EXPECT_TRUE(output.find("folder1/") != std::string::npos);
    EXPECT_TRUE(output.find("folder2/") != std::string::npos);
  }

  TEST_F(TabCompletionTest, TabCompleteAfterParentNavigation)
  {
    loginAsUser();
    navigateToSubfolder1(); // Now in /folder1/subfolder1

    _terminal.queueInput("../../folder2/t");
    _service->service();
    pressTab();

    std::string output = _terminal.getOutput();
    EXPECT_TRUE(output.find("target1/") != std::string::npos);
    EXPECT_TRUE(output.find("target2/") != std::string::npos);
  }

  TEST_F(TabCompletionTest, TabCompleteWithMultiLevelParentNavigation)
  {
    loginAsUser();
    navigateToSubfolder1();

    _terminal.queueInput("deep\n");
    _service->service();
    _terminal.clearOutput();

    _terminal.queueInput("../../../f");
    _service->service();
    pressTab();

    std::string output = _terminal.getOutput();
    EXPECT_TRUE(output.find("folder1/") != std::string::npos);
    EXPECT_TRUE(output.find("folder2/") != std::string::npos);
  }

  TEST_F(TabCompletionTest, TabCompleteWithExtraParentDirectories)
  {
    loginAsUser();
    navigateToSubfolder1();

    _terminal.queueInput("../../../../folder2/t");
    _service->service();
    pressTab();

    std::string output = _terminal.getOutput();
    EXPECT_TRUE(output.find("target1/") != std::string::npos);
    EXPECT_TRUE(output.find("target2/") != std::string::npos);
  }

  TEST_F(TabCompletionTest, TabCompletePartialParentDirectory)
  {
    loginAsUser();
    navigateToSubfolder1();

    _terminal.queueInput("../s");
    _service->service();
    pressTab();

    std::string output = _terminal.getOutput();
    EXPECT_TRUE(output.find("subfolder2") != std::string::npos);
  }

  TEST_F(TabCompletionTest, TabCompleteAfterDotDirectory)
  {
    loginAsUser();
    navigateToSubfolder1();

    _terminal.queueInput("./../s");
    _service->service();
    pressTab();

    std::string output = _terminal.getOutput();
    EXPECT_TRUE(output.find("subfolder2") != std::string::npos);
  }

  TEST_F(TabCompletionTest, TabCompleteNonexistentParentPath)
  {
    loginAsUser();
    navigateToSubfolder1();

    _terminal.queueInput("../nonexistent/s");
    _service->service();
    pressTab();

    std::string output = _terminal.getOutput();
    EXPECT_FALSE(output.find("subfolder") != std::string::npos);
  }

  TEST_F(TabCompletionTest, TabCompleteWithMixedPaths)
  {
    loginAsUser();
    navigateToSubfolder1();

    _terminal.queueInput("./../subfolder2/../subfolder1/d");
    _service->service();
    pressTab();

    std::string output = _terminal.getOutput();
    EXPECT_TRUE(output.find("deep") != std::string::npos);
  }

  TEST_F(TabCompletionTest, TabCompleteParentDirectoryFromRoot)
  {
    loginAsUser();

    _terminal.queueInput("../f");
    _service->service();
    pressTab();

    std::string output = _terminal.getOutput();
    EXPECT_TRUE(output.find("folder1/") != std::string::npos);
    EXPECT_TRUE(output.find("folder2/") != std::string::npos);
  }

}
