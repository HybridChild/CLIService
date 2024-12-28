#include <gtest/gtest.h>
#include "cliService/tree/PathCompleter.hpp"
#include "cliService/tree/Directory.hpp"

namespace cliService
{

  enum class AccessLevel
  {
    User,
    Admin
  };

  class TestCommand : public CommandIf
  {
  public:
    TestCommand(std::string name, AccessLevel level, std::string description = "")
      : CommandIf(std::move(name), level, std::move(description))
    {}

    CLIResponse execute(const std::vector<std::string>&) override {
      return CLIResponse::success();
    }
  };

  class PathCompleterAdvancedTest : public ::testing::Test
  {
  protected:
    void SetUp() override {
      root = std::make_unique<Directory>("root", AccessLevel::User);

      // Create a more complex directory structure:
      // /
      // ├── test/
      // │   ├── test1/
      // │   ├── test2/
      // │   └── testing/
      // ├── admin/
      // │   ├── conf/ (Admin)
      // │   └── logs/ (Admin)
      // └── utils/
      //     ├── tool1
      //     ├── tool2
      //     └── common/
      
      auto& test = root->addDynamicDirectory("test", AccessLevel::User);
      test.addDynamicDirectory("test1", AccessLevel::User);
      test.addDynamicDirectory("test2", AccessLevel::User);
      test.addDynamicDirectory("testing", AccessLevel::User);
      
      auto& admin = root->addDynamicDirectory("admin", AccessLevel::Admin);
      admin.addDynamicDirectory("conf", AccessLevel::Admin);
      admin.addDynamicDirectory("logs", AccessLevel::Admin);
      
      auto& utils = root->addDynamicDirectory("utils", AccessLevel::User);
      utils.addDynamicCommand<TestCommand>("tool1", AccessLevel::User);
      utils.addDynamicCommand<TestCommand>("tool2", AccessLevel::User);
      utils.addDynamicDirectory("common", AccessLevel::User);
    }

    std::unique_ptr<Directory> root;
  };

  TEST_F(PathCompleterAdvancedTest, MultiplePartialMatches)
  {
    auto result = PathCompleter::complete(*root, "test/test", AccessLevel::User);

    EXPECT_EQ(result.matchedNode, "test");  // Common prefix
    ASSERT_EQ(result.allOptions.size(), 3);
    EXPECT_EQ(result.allOptions[0], "test1/");
    EXPECT_EQ(result.allOptions[1], "test2/");
    EXPECT_EQ(result.allOptions[2], "testing/");
  }

  TEST_F(PathCompleterAdvancedTest, AccessLevelFiltering)
  {
    // Test as User
    {
      auto result = PathCompleter::complete(*root, "admin/", AccessLevel::User);
      EXPECT_TRUE(result.allOptions.empty());
    }

    // Test as Admin
    {
      auto result = PathCompleter::complete(*root, "admin/", AccessLevel::Admin);
      ASSERT_EQ(result.allOptions.size(), 2);
      EXPECT_EQ(result.allOptions[0], "conf/");
      EXPECT_EQ(result.allOptions[1], "logs/");
    }
  }

  TEST_F(PathCompleterAdvancedTest, CompletionAtRoot)
  {
    auto result = PathCompleter::complete(*root, "", AccessLevel::User);

    ASSERT_EQ(result.allOptions.size(), 2); // test/, utils/, (admin/ filtered by access)
    EXPECT_EQ(result.allOptions[0], "test/");
    EXPECT_EQ(result.allOptions[1], "utils/");
  }

  TEST_F(PathCompleterAdvancedTest, InvalidPathCompletion)
  {
    auto result = PathCompleter::complete(*root, "nonexistent/", AccessLevel::User);

    EXPECT_TRUE(result.allOptions.empty());
    EXPECT_TRUE(result.fillCharacters.empty());
  }

  TEST_F(PathCompleterAdvancedTest, DoubleSlashHandling)
  {
    auto result = PathCompleter::complete(*root, "test//test", AccessLevel::User);

    EXPECT_FALSE(result.allOptions.empty());
    ASSERT_EQ(result.allOptions.size(), 3);
    // Should handle double slashes same as single slash
  }

  TEST_F(PathCompleterAdvancedTest, MixedCommandAndDirectoryCompletion)
  {
    auto result = PathCompleter::complete(*root, "utils/t", AccessLevel::User);

    ASSERT_EQ(result.allOptions.size(), 2);
    EXPECT_EQ(result.allOptions[0], "tool1");
    EXPECT_EQ(result.allOptions[1], "tool2");
  }

  TEST_F(PathCompleterAdvancedTest, CompletionWithTrailingSlash)
  {
    auto result = PathCompleter::complete(*root, "test/test1/", AccessLevel::User);

    EXPECT_TRUE(result.allOptions.empty());  // No completions in empty directory
  }

  TEST_F(PathCompleterAdvancedTest, AbsoluteVsRelativePaths)
  {
    // Absolute path
    auto absResult = PathCompleter::complete(*root, "/test/", AccessLevel::User);

    // Relative path
    auto relResult = PathCompleter::complete(*root, "test/", AccessLevel::User);

    // Should give same results
    EXPECT_EQ(absResult.allOptions, relResult.allOptions);
  }


  class ParentPathCompletionTest : public ::testing::Test
  {
  protected:
    void SetUp() override {
      root = std::make_unique<Directory>("root", AccessLevel::User);

      // Create a complex directory structure for testing relative navigation:
      // /
      // ├── folder1/
      // │   ├── subfolder1/
      // │   │   └── deep/
      // │   └── subfolder2/
      // └── folder2/
      //     ├── target1/
      //     │   └── item
      //     └── target2/
      //         ├── item1
      //         └── item2
      
      auto& folder1 = root->addDynamicDirectory("folder1", AccessLevel::User);
      auto& subfolder1 = folder1.addDynamicDirectory("subfolder1", AccessLevel::User);
      subfolder1.addDynamicDirectory("deep", AccessLevel::User);
      folder1.addDynamicDirectory("subfolder2", AccessLevel::User);
      
      auto& folder2 = root->addDynamicDirectory("folder2", AccessLevel::User);
      auto& target1 = folder2.addDynamicDirectory("target1", AccessLevel::User);
      target1.addDynamicCommand<TestCommand>("item", AccessLevel::User);
      
      auto& target2 = folder2.addDynamicDirectory("target2", AccessLevel::User);
      target2.addDynamicCommand<TestCommand>("item1", AccessLevel::User);
      target2.addDynamicCommand<TestCommand>("item2", AccessLevel::User);

      // Keep reference to test directory for relative path testing
      testDir = &subfolder1;
    }

    std::unique_ptr<Directory> root;
    Directory* testDir;  // Reference to /folder1/subfolder1
  };

  TEST_F(ParentPathCompletionTest, SimpleParentNavigation)
  {
    // From /folder1/subfolder1, typing "../sub" should match both subfolder1 and subfolder2
    auto result = PathCompleter::complete(*testDir, "../sub", AccessLevel::User);

    // Should find both subfolders
    ASSERT_EQ(result.allOptions.size(), 2);
    EXPECT_EQ(result.allOptions[0], "subfolder1/");
    EXPECT_EQ(result.allOptions[1], "subfolder2/");

    // Should match their common prefix
    EXPECT_EQ(result.matchedNode, "subfolder");
    
    // Should provide the remaining characters of the common prefix after "sub"
    EXPECT_EQ(result.fillCharacters, "folder");
    
    // Should indicate these are directories
    EXPECT_TRUE(result.isDirectory);
  }

  TEST_F(ParentPathCompletionTest, DoubleParentNavigation)
  {
    // From /folder1/subfolder1, completing "../../f" should show both folder1 and folder2
    auto result = PathCompleter::complete(*testDir, "../../f", AccessLevel::User);

    ASSERT_EQ(result.allOptions.size(), 2);
    EXPECT_EQ(result.allOptions[0], "folder1/");
    EXPECT_EQ(result.allOptions[1], "folder2/");
    EXPECT_EQ(result.matchedNode, "folder");
    EXPECT_EQ(result.fillCharacters, "older");
  }

  TEST_F(ParentPathCompletionTest, ParentWithDeepPath)
  {
    // From /folder1/subfolder1, completing "../../folder2/target" should show target1 and target2
    auto result = PathCompleter::complete(*testDir, "../../folder2/target", AccessLevel::User);

    ASSERT_EQ(result.allOptions.size(), 2);
    EXPECT_EQ(result.allOptions[0], "target1/");
    EXPECT_EQ(result.allOptions[1], "target2/");
    EXPECT_EQ(result.matchedNode, "target");
    EXPECT_TRUE(result.isDirectory);
  }

  TEST_F(ParentPathCompletionTest, MultipleParentsThenDeep)
  {
    // From /folder1/subfolder1/deep, completing "../../../folder2/target2/i" should show item1 and item2
    auto* deepDir = testDir->findNode({"deep"});
    ASSERT_NE(deepDir, nullptr);

    auto result = PathCompleter::complete(*static_cast<Directory*>(deepDir), 
                                        "../../../folder2/target2/i", 
                                        AccessLevel::User);

    ASSERT_EQ(result.allOptions.size(), 2);
    EXPECT_EQ(result.allOptions[0], "item1");
    EXPECT_EQ(result.allOptions[1], "item2");
    EXPECT_EQ(result.matchedNode, "item");
  }

  TEST_F(ParentPathCompletionTest, ParentPastRoot)
  {
    // Going past root should still work and normalize the path
    auto result = PathCompleter::complete(*testDir, "../../../../../../../folder", AccessLevel::User);

    ASSERT_EQ(result.allOptions.size(), 2);
    EXPECT_EQ(result.allOptions[0], "folder1/");
    EXPECT_EQ(result.allOptions[1], "folder2/");
  }

  TEST_F(ParentPathCompletionTest, MixedRelativeAndParentPaths)
  {
    // Test a complex path mixing ./ and ../
    auto result = PathCompleter::complete(*testDir, "./../subfolder2/../subfolder1/d", AccessLevel::User);

    ASSERT_EQ(result.allOptions.size(), 1);
    EXPECT_EQ(result.allOptions[0], "deep/");
    EXPECT_EQ(result.matchedNode, "deep");
  }

  TEST_F(ParentPathCompletionTest, CompletionAfterExactParentMatch)
  {
    // Test completion when the path up to the last segment is exact
    auto result = PathCompleter::complete(*testDir, "../../folder2/target2/", AccessLevel::User);

    ASSERT_EQ(result.allOptions.size(), 2);
    EXPECT_EQ(result.allOptions[0], "item1");
    EXPECT_EQ(result.allOptions[1], "item2");
  }

  TEST_F(ParentPathCompletionTest, NonexistentParentPath)
  {
    // Test Directory Structure:
    // /
    // ├── folder1/
    // │   ├── subfolder1/  <-- We are here (testDir)
    // │   │   └── deep/
    // │   └── subfolder2/
    // └── folder2/
    
    // Input path breakdown:
    // 1. "../"          -> go up to /folder1
    // 2. "nonexistent/" -> attempt to enter directory that doesn't exist
    // 3. "../"          -> attempt to go up from nonexistent directory (invalid)
    // 4. "sub"          -> attempt to complete something starting with "sub"
    
    auto result = PathCompleter::complete(*testDir, "../nonexistent/../sub", AccessLevel::User);

    // Verify the completion result is empty
    EXPECT_TRUE(result.allOptions.empty()) 
        << "Expected no completion options when path contains nonexistent directory";
    
    // Verify no auto-completion text is suggested
    EXPECT_TRUE(result.fillCharacters.empty()) 
        << "Expected no fill characters when path contains nonexistent directory";
    
    // Verify other completion result fields are empty/default
    EXPECT_TRUE(result.fullPath.empty()) 
        << "Expected empty full path for invalid path completion";
    EXPECT_TRUE(result.matchedNode.empty()) 
        << "Expected no matched node for invalid path completion";
    EXPECT_FALSE(result.isDirectory) 
        << "Expected isDirectory to be false for invalid path completion";
  }

  TEST_F(ParentPathCompletionTest, ParentNavigationWithSimilarNames)
  {
    // Add some directories with similar names to test precise matching
    auto& folder1 = static_cast<Directory&>(*root->findNode({"folder1"}));
    folder1.addDynamicDirectory("similar", AccessLevel::User);
    folder1.addDynamicDirectory("similar2", AccessLevel::User);

    auto result = PathCompleter::complete(*testDir, "../sim", AccessLevel::User);

    ASSERT_EQ(result.allOptions.size(), 2);
    EXPECT_EQ(result.allOptions[0], "similar/");
    EXPECT_EQ(result.allOptions[1], "similar2/");
    EXPECT_EQ(result.matchedNode, "similar");
  }

}
