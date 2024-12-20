#include "gtest/gtest.h"
#include "cliService/cli/CLIService.hpp"
#include "mock/command/CommandMock.hpp"
#include "mock/io/CharIOStreamMock.hpp"

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
          CLIServiceConfiguration{_ioStream, std::move(users), std::move(root), HISTORY_SIZE});
    }

    void loginAsUser()
    {
      _ioStream.queueInput("user:user123\n");
      _service->activate();
      _service->service();
      _ioStream.clearOutput();
    }

    void navigateToSubfolder1()
    {
      _ioStream.queueInput("folder1/subfolder1\n");
      _service->service();
      _ioStream.clearOutput();
    }

    void pressTab()
    {
      _ioStream.queueInput("\t");
      _service->service();
    }

    CharIOStreamMock _ioStream;
    std::unique_ptr<CLIService> _service;
  };

  TEST_F(TabCompletionTest, DISABLED_TabCompleteParentDirectory)
  {
    loginAsUser();
    navigateToSubfolder1(); // Now in /folder1/subfolder1

    _ioStream.queueInput("../../f");
    _service->service();
    pressTab();

    std::string output = _ioStream.getOutput();
    EXPECT_TRUE(output.find("folder1/") != std::string::npos);
    EXPECT_TRUE(output.find("folder2/") != std::string::npos);
  }

  TEST_F(TabCompletionTest, DISABLED_TabCompleteAfterParentNavigation)
  {
    loginAsUser();
    navigateToSubfolder1(); // Now in /folder1/subfolder1

    _ioStream.queueInput("../../folder2/t");
    _service->service();
    pressTab();

    std::string output = _ioStream.getOutput();
    EXPECT_TRUE(output.find("target1/") != std::string::npos);
    EXPECT_TRUE(output.find("target2/") != std::string::npos);
  }

  TEST_F(TabCompletionTest, DISABLED_TabCompleteWithMultiLevelParentNavigation)
  {
    loginAsUser();
    navigateToSubfolder1();

    _ioStream.queueInput("deep\n");
    _service->service();
    _ioStream.clearOutput();

    _ioStream.queueInput("../../../f");
    _service->service();
    pressTab();

    std::string output = _ioStream.getOutput();
    EXPECT_TRUE(output.find("folder1/") != std::string::npos);
    EXPECT_TRUE(output.find("folder2/") != std::string::npos);
  }

  TEST_F(TabCompletionTest, DISABLED_TabCompleteWithExtraParentDirectories)
  {
    loginAsUser();
    navigateToSubfolder1();

    _ioStream.queueInput("../../../../folder2/t");
    _service->service();
    pressTab();

    std::string output = _ioStream.getOutput();
    EXPECT_TRUE(output.find("target1/") != std::string::npos);
    EXPECT_TRUE(output.find("target2/") != std::string::npos);
  }

  TEST_F(TabCompletionTest, DISABLED_TabCompletePartialParentDirectory)
  {
    loginAsUser();
    navigateToSubfolder1();

    _ioStream.queueInput("../s");
    _service->service();
    pressTab();

    std::string output = _ioStream.getOutput();
    EXPECT_TRUE(output.find("subfolder2") != std::string::npos);
  }

  TEST_F(TabCompletionTest, DISABLED_TabCompleteAfterDotDirectory)
  {
    loginAsUser();
    navigateToSubfolder1();

    _ioStream.queueInput("./../s");
    _service->service();
    pressTab();

    std::string output = _ioStream.getOutput();
    EXPECT_TRUE(output.find("subfolder2") != std::string::npos);
  }

  TEST_F(TabCompletionTest, DISABLED_TabCompleteNonexistentParentPath)
  {
    loginAsUser();
    navigateToSubfolder1();

    _ioStream.queueInput("../nonexistent/s");
    _service->service();
    pressTab();

    std::string output = _ioStream.getOutput();
    EXPECT_FALSE(output.find("subfolder") != std::string::npos);
  }

  TEST_F(TabCompletionTest, DISABLED_TabCompleteWithMixedPaths)
  {
    loginAsUser();
    navigateToSubfolder1();

    _ioStream.queueInput("./../subfolder2/../subfolder1/d");
    _service->service();
    pressTab();

    std::string output = _ioStream.getOutput();
    EXPECT_TRUE(output.find("deep") != std::string::npos);
  }

  TEST_F(TabCompletionTest, DISABLED_TabCompleteParentDirectoryFromRoot)
  {
    loginAsUser();

    _ioStream.queueInput("../f");
    _service->service();
    pressTab();

    std::string output = _ioStream.getOutput();
    EXPECT_TRUE(output.find("folder1/") != std::string::npos);
    EXPECT_TRUE(output.find("folder2/") != std::string::npos);
  }

}
