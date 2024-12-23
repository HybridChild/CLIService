#include "gtest/gtest.h"
#include "cliService/cli/CLIService.hpp"
#include "mock/command/CommandMock.hpp"
#include "mock/io/CharIOStreamMock.hpp"

namespace cliService
{

  namespace
  {
    // Helper function to check if a string ends with a specific suffix
    bool stringEndsWith(const std::string& str, const std::string& suffix)
    {
      if (str.length() < suffix.length()) {
        return false;
      }
      return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    }
  }

  // This test suite focuses on path resolution correctness for tab completion,
  // complementing the output formatting tests in TabCompletionStreamTest
  class TabCompletionTest : public ::testing::Test
  {
    static constexpr size_t HISTORY_SIZE = 10;
    static constexpr uint32_t INPUT_TIMEOUT_MS = 1000;

  protected:
    void SetUp() override
    {
      // Create a test directory structure:
      // root/
      // ├── folder1/
      // │   ├── subfolder1/
      // │   │   └── deep/
      // │   └── subfolder2/
      // └── folder2/
      //     ├── target1/
      //     └── target2/

      auto root = std::make_unique<Directory>("root", AccessLevel::User);

      auto &folder1 = root->addDynamicDirectory("folder1", AccessLevel::User);
      auto &subfolder1 = folder1.addDynamicDirectory("subfolder1", AccessLevel::User);
      subfolder1.addDynamicDirectory("deep", AccessLevel::User);
      folder1.addDynamicDirectory("subfolder2", AccessLevel::User);

      auto &folder2 = root->addDynamicDirectory("folder2", AccessLevel::User);
      folder2.addDynamicDirectory("target1", AccessLevel::User);
      folder2.addDynamicDirectory("target2", AccessLevel::User);

      std::vector<User> users = {
          {"admin", "admin123", AccessLevel::Admin},
          {"user", "user123", AccessLevel::User}};

      _service = std::make_unique<CLIService>(
          CLIServiceConfiguration{_ioStream, std::move(users), std::move(root), INPUT_TIMEOUT_MS, HISTORY_SIZE});
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

    // Returns true if both folder1 and folder2 are found in completion results
    bool verifyBothFoldersFound(const std::string& output) const
    {
      return output.find("folder1/") != std::string::npos && 
             output.find("folder2/") != std::string::npos;
    }

    // Returns true if both target1 and target2 are found in completion results
    bool verifyBothTargetsFound(const std::string& output) const
    {
      return output.find("target1/") != std::string::npos && 
             output.find("target2/") != std::string::npos;
    }

    void pressTab()
    {
      _ioStream.queueInput("\t");
      _service->service();
    }

    CharIOStreamMock _ioStream;
    std::unique_ptr<CLIService> _service;
  };

  TEST_F(TabCompletionTest, ResolvesBasicParentDirectoryPath)
  {
    loginAsUser();
    navigateToSubfolder1(); // Now in /folder1/subfolder1

    _ioStream.queueInput("../../f");
    _service->service();
    pressTab();

    std::string output = _ioStream.getOutput();
    EXPECT_TRUE(verifyBothFoldersFound(output)) 
      << "Parent directory resolution should find both folders";
  }

  TEST_F(TabCompletionTest, ResolvesPathAfterParentNavigation)
  {
    loginAsUser();
    navigateToSubfolder1(); // Now in /folder1/subfolder1

    _ioStream.queueInput("../../folder2/t");
    _service->service();
    pressTab();

    std::string output = _ioStream.getOutput();
    EXPECT_TRUE(verifyBothTargetsFound(output))
      << "Should resolve targets after navigating through parent directory";
  }

  TEST_F(TabCompletionTest, ResolvesMultiLevelParentNavigation)
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
    EXPECT_TRUE(verifyBothFoldersFound(output))
      << "Should resolve multiple levels of parent navigation";
  }

  TEST_F(TabCompletionTest, HandlesExcessParentDirectories)
  {
    loginAsUser();
    navigateToSubfolder1();

    _ioStream.queueInput("../../../../folder2/t");
    _service->service();
    pressTab();

    std::string output = _ioStream.getOutput();
    EXPECT_TRUE(verifyBothTargetsFound(output))
      << "Should handle excess parent directories by capping at root";
  }

  TEST_F(TabCompletionTest, ResolvesPartialParentDirectoryPath)
  {
    loginAsUser();
    navigateToSubfolder1();

    _ioStream.queueInput("../s");
    _service->service();
    pressTab();

    std::string output = _ioStream.getOutput();
    EXPECT_TRUE(output.find("subfolder2") != std::string::npos)
      << "Should resolve partial paths in parent directory";
  }

  TEST_F(TabCompletionTest, HandlesPathWithDotDirectory)
  {
    loginAsUser();
    navigateToSubfolder1();

    _ioStream.queueInput("./../s");
    _service->service();
    pressTab();

    std::string output = _ioStream.getOutput();
    EXPECT_TRUE(output.find("subfolder2") != std::string::npos)
      << "Should resolve paths containing current directory reference";
  }

  TEST_F(TabCompletionTest, HandlesNonexistentParentPath)
  {
    loginAsUser();
    navigateToSubfolder1();

    _ioStream.queueInput("../nonexistent/s");
    _service->service();
    pressTab();

    std::string output = _ioStream.getOutput();
    EXPECT_FALSE(output.find("subfolder") != std::string::npos)
      << "Should not resolve completions in nonexistent paths";
  }

  TEST_F(TabCompletionTest, ResolvesMixedPathNavigation)
  {
    loginAsUser();
    navigateToSubfolder1();

    _ioStream.queueInput("./../subfolder2/../subfolder1/d");
    _service->service();
    pressTab();

    std::string output = _ioStream.getOutput();
    EXPECT_TRUE(output.find("deep") != std::string::npos)
      << "Should resolve mixed path navigation correctly";
  }

  TEST_F(TabCompletionTest, HandlesParentDirectoryFromRoot)
  {
    loginAsUser();

    _ioStream.queueInput("../f");
    _service->service();
    pressTab();

    std::string output = _ioStream.getOutput();
    EXPECT_TRUE(verifyBothFoldersFound(output))
      << "Should handle parent directory from root appropriately";
  }


  // This test suite focuses on stream output formatting and display behavior
  class TabCompletionStreamTest : public ::testing::Test
  {
    static constexpr size_t HISTORY_SIZE = 10;
    static constexpr uint32_t INPUT_TIMEOUT_MS = 1000;

  protected:
    void SetUp() override
    {
      // Create a test directory structure:
      // root/
      // ├── folder1/
      // │   ├── subfolder1/
      // │   │   └── deep/
      // │   │       └── nested/
      // │   ├── subfolder2/
      // │   └── similar1/
      // ├── folder2/
      // │   ├── target1/
      // │   └── target2/
      // └── commands/
      //     ├── cmd1
      //     ├── cmd2
      //     └── common

      auto root = std::make_unique<Directory>("root", AccessLevel::User);

      auto& folder1 = root->addDynamicDirectory("folder1", AccessLevel::User);
      auto& subfolder1 = folder1.addDynamicDirectory("subfolder1", AccessLevel::User);
      auto& deep = subfolder1.addDynamicDirectory("deep", AccessLevel::User);
      deep.addDynamicDirectory("nested", AccessLevel::User);
      folder1.addDynamicDirectory("subfolder2", AccessLevel::User);
      folder1.addDynamicDirectory("similar1", AccessLevel::User);

      auto& folder2 = root->addDynamicDirectory("folder2", AccessLevel::User);
      folder2.addDynamicDirectory("target1", AccessLevel::User);
      folder2.addDynamicDirectory("target2", AccessLevel::User);

      auto& commands = root->addDynamicDirectory("commands", AccessLevel::User);
      commands.addDynamicCommand<CommandMock>("cmd1", AccessLevel::User, "Test command 1");
      commands.addDynamicCommand<CommandMock>("cmd2", AccessLevel::User, "Test command 2");
      commands.addDynamicCommand<CommandMock>("common", AccessLevel::User, "Common command");

      std::vector<User> users = {
        {"admin", "admin123", AccessLevel::Admin},
        {"user", "user123", AccessLevel::User}
      };

      _service = std::make_unique<CLIService>(
        CLIServiceConfiguration{_ioStream, std::move(users), std::move(root), INPUT_TIMEOUT_MS, HISTORY_SIZE}
      );
    }

    void loginAsUser()
    {
      _ioStream.queueInput("user:user123\r\n");
      _service->activate();
      _service->service();
      
      // Verify login sequence messages
      std::string output = _ioStream.getOutput();
      EXPECT_TRUE(output.find("Welcome to CLI Service. Please login.") != std::string::npos);
      EXPECT_TRUE(output.find("Logged in. Type 'help' for help.") != std::string::npos);
      EXPECT_TRUE(stringEndsWith(output, "user@/> "))
        << "Expected output to end with 'user@/> '";
    }

    void navigateTo(const std::string& path)
    {
      _ioStream.queueInput(path + "\r\n");
      _service->service();
    }

    void pressTab()
    {
      _ioStream.queueInput("\t");
      _service->service();
    }

    CharIOStreamMock _ioStream;
    std::unique_ptr<CLIService> _service;
  };


  TEST_F(TabCompletionStreamTest, SingleMatchAppendsDirSlash)
  {
    loginAsUser();
    std::string initialOutput = _ioStream.getOutput();
    _ioStream.clearOutput();
    
    _ioStream.queueInput("folder1");
    _service->service();
    pressTab();

    std::string completionOutput = _ioStream.getOutput();
    
    // Should only append the slash, no newlines or other output
    EXPECT_EQ(completionOutput, "folder1/")
      << "Output should only append '/' for single directory match";
    
    // Full output should end with completed path
    std::string fullOutput = initialOutput + completionOutput;
    EXPECT_TRUE(stringEndsWith(fullOutput, "user@/> folder1/"))
      << "Final output should end with 'user@/> folder1/'";
  }

  TEST_F(TabCompletionStreamTest, MultipleMatchesShowsAllOptions)
  {
    loginAsUser();
    std::string initialOutput = _ioStream.getOutput();
    _ioStream.clearOutput();

    _ioStream.queueInput("f");
    _service->service();
    pressTab();

    std::string completionOutput = _ioStream.getOutput();
    
    // Verify exact output sequence:
    // 1. Should show original input followed by newline
    EXPECT_TRUE(completionOutput.find("f\r\n") == 0) 
        << "Output should start with original input 'f' followed by newline";
      
    // 2. Should show options with proper spacing
    size_t optionsPos = completionOutput.find("   folder1/   folder2/");
    EXPECT_NE(optionsPos, std::string::npos)
        << "Should show all matches with consistent spacing";
      
    // 3. Should show another newline after options
    size_t afterOptionsPos = optionsPos + strlen("   folder1/   folder2/");
    EXPECT_NE(completionOutput.find("\r\n", afterOptionsPos), std::string::npos)
        << "Should show newline after options";
      
    // 4. Should show prompt with common prefix completed
    EXPECT_TRUE(stringEndsWith(completionOutput, "user@/> folder"))
        << "Output should end with prompt and completed common prefix 'folder'";

    // 5. Verify the complete sequence
    std::string expectedSequence = 
        "f\r\n"                    // Original input
        "   folder1/   folder2/\r\n"  // Options
        "user@/> folder";         // New prompt with completion
    
    // Remove any trailing whitespace from completionOutput for exact comparison
    std::string trimmedOutput = completionOutput;
    while (!trimmedOutput.empty() && std::isspace(trimmedOutput.back())) {
        trimmedOutput.pop_back();
    }
    
    EXPECT_EQ(trimmedOutput, expectedSequence)
        << "Complete output sequence should exactly match expected format";
  }

  TEST_F(TabCompletionStreamTest, PartialMatchCompletion)
  {
    loginAsUser();
    navigateTo("folder1");
    std::string initialOutput = _ioStream.getOutput();
    _ioStream.clearOutput();

    _ioStream.queueInput("sub");
    _service->service();
    pressTab();

    std::string completionOutput = _ioStream.getOutput();
    
    // Verify exact output sequence:
    EXPECT_TRUE(completionOutput.find("sub\r\n") == 0)
      << "Output should start with newline";
      
    EXPECT_TRUE(completionOutput.find("   subfolder1/   subfolder2/") != std::string::npos)
      << "Should show matching subfolders with consistent spacing";
      
    size_t optionsPos = completionOutput.find("   subfolder1/   subfolder2/");
    size_t afterOptionsPos = optionsPos + strlen("   subfolder1/   subfolder2/");
    EXPECT_TRUE(completionOutput.find("\r\n", afterOptionsPos) != std::string::npos)
      << "Should show newline after options";
      
    EXPECT_TRUE(stringEndsWith(completionOutput, "user@/folder1> subfolder"))
      << "Output should end with 'user@/folder1> subfolder'";
  }

  TEST_F(TabCompletionStreamTest, CommandCompletion)
  {
    loginAsUser();
    navigateTo("commands");
    std::string initialOutput = _ioStream.getOutput();
    _ioStream.clearOutput();

    _ioStream.queueInput("c");
    _service->service();
    pressTab();

    std::string completionOutput = _ioStream.getOutput();
    
    // Verify sequence for command completion:
    EXPECT_TRUE(completionOutput.find("c\r\n") == 0)
      << "Output should start with newline";
      
    EXPECT_TRUE(completionOutput.find("   cmd1   cmd2   common") != std::string::npos)
      << "Should show commands without slashes and with consistent spacing";
      
    size_t optionsPos = completionOutput.find("   cmd1   cmd2   common");
    size_t afterOptionsPos = optionsPos + strlen("   cmd1   cmd2   common");
    EXPECT_TRUE(completionOutput.find("\r\n", afterOptionsPos) != std::string::npos)
      << "Should show newline after options";
      
    EXPECT_TRUE(stringEndsWith(completionOutput, "user@/commands> c"))
      << "Output should end with 'user@/commands> c'";
  }

  TEST_F(TabCompletionStreamTest, ParentDirectoryNavigationFormatting)
  {
    loginAsUser();
    navigateTo("folder1/subfolder1/deep");
    std::string initialOutput = _ioStream.getOutput();
    _ioStream.clearOutput();

    _ioStream.queueInput("../../sub");
    _service->service();
    pressTab();

    std::string completionOutput = _ioStream.getOutput();
    
    // Verify sequence for parent directory navigation:
    EXPECT_TRUE(completionOutput.find("../../sub\r\n") == 0)
      << "Output should start with newline";
      
    EXPECT_TRUE(completionOutput.find("   subfolder1/   subfolder2/") != std::string::npos)
      << "Should show all valid targets with consistent spacing";
      
    size_t optionsPos = completionOutput.find("   subfolder1/   subfolder2/");
    size_t afterOptionsPos = optionsPos + strlen("   subfolder1/   subfolder2/");
    EXPECT_TRUE(completionOutput.find("\r\n", afterOptionsPos) != std::string::npos)
      << "Should show newline after options";
      
    EXPECT_TRUE(stringEndsWith(completionOutput, "user@/folder1/subfolder1/deep> ../../subfolder"))
      << "Output should end with original path and input";
  }

  TEST_F(TabCompletionStreamTest, AbsolutePathCompletionFormatting)
  {
    loginAsUser();
    navigateTo("folder1/subfolder1");
    std::string initialOutput = _ioStream.getOutput();
    _ioStream.clearOutput();

    _ioStream.queueInput("/folder2/t");
    _service->service();
    pressTab();

    std::string completionOutput = _ioStream.getOutput();
    
    // Verify sequence for absolute path completion:
    EXPECT_TRUE(completionOutput.find("/folder2/t\r\n") == 0)
      << "Output should start with newline";
      
    EXPECT_TRUE(completionOutput.find("   target1/   target2/") != std::string::npos)
      << "Should show targets with consistent spacing";
      
    size_t optionsPos = completionOutput.find("   target1/   target2/");
    size_t afterOptionsPos = optionsPos + strlen("   target1/   target2/");
    EXPECT_TRUE(completionOutput.find("\r\n", afterOptionsPos) != std::string::npos)
      << "Should show newline after options";
      
    EXPECT_TRUE(stringEndsWith(completionOutput, "user@/folder1/subfolder1> /folder2/target"))
      << "Output should end with current directory and absolute path input";
  }

  TEST_F(TabCompletionStreamTest, DeepNestedCompletion)
  {
    loginAsUser();
    navigateTo("folder1/subfolder1");
    std::string initialOutput = _ioStream.getOutput();
    _ioStream.clearOutput();

    _ioStream.queueInput("deep/n");
    _service->service();
    pressTab();

    std::string completionOutput = _ioStream.getOutput();
    
    // Verify sequence for deep nested completion:
    // For single match, should only append completion
    EXPECT_TRUE(completionOutput.find("deep/nested/") == 0)
      << "Output should start with completion 'ested/'";
      
    std::string fullOutput = initialOutput + completionOutput;
    EXPECT_TRUE(stringEndsWith(fullOutput, "user@/folder1/subfolder1> deep/nested/"))
      << "Final output should show complete nested path";
  }

  TEST_F(TabCompletionStreamTest, NoMatchesNoOutput)
  {
    loginAsUser();
    std::string initialOutput = _ioStream.getOutput();
    _ioStream.clearOutput();

    _ioStream.queueInput("nonexistent");
    _service->service();
    pressTab();

    std::string completionOutput = _ioStream.getOutput();
    EXPECT_TRUE(completionOutput.find("nonexistent") == 0)
      << "Output should only show the original input";
      
    std::string fullOutput = initialOutput + completionOutput;
    EXPECT_TRUE(stringEndsWith(fullOutput, "user@/> nonexistent"))
      << "Final output should show unchanged input";
  }

  TEST_F(TabCompletionStreamTest, CommonPrefixCompletion)
  {
    loginAsUser();
    navigateTo("commands");
    std::string initialOutput = _ioStream.getOutput();
    _ioStream.clearOutput();

    _ioStream.queueInput("cm");
    _service->service();
    pressTab();

    std::string completionOutput = _ioStream.getOutput();
    
    // First should show the common prefix completion
    EXPECT_TRUE(completionOutput.find("cm") == 0)
      << "Output should start with the typed characters";
      
    // Then show options on new line
    EXPECT_TRUE(completionOutput.find("\r\n") != std::string::npos)
      << "Should show newline after prefix completion";
      
    EXPECT_TRUE(completionOutput.find("   cmd1   cmd2") != std::string::npos)
      << "Should show remaining options with consistent spacing";
      
    size_t optionsPos = completionOutput.find("   cmd1   cmd2");
    size_t afterOptionsPos = optionsPos + strlen("   cmd1   cmd2");
    EXPECT_TRUE(completionOutput.find("\r\n", afterOptionsPos) != std::string::npos)
      << "Should show newline after options";
      
    EXPECT_TRUE(stringEndsWith(completionOutput, "user@/commands> cmd"))
      << "Output should end with completed prefix";
  }

}
