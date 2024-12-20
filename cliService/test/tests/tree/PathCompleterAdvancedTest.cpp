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

    CommandResponse execute(const std::vector<std::string>&) override {
      return CommandResponse::success();
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
      
      auto& test = root->addDirectory("test", AccessLevel::User);
      test.addDirectory("test1", AccessLevel::User);
      test.addDirectory("test2", AccessLevel::User);
      test.addDirectory("testing", AccessLevel::User);
      
      auto& admin = root->addDirectory("admin", AccessLevel::Admin);
      admin.addDirectory("conf", AccessLevel::Admin);
      admin.addDirectory("logs", AccessLevel::Admin);
      
      auto& utils = root->addDirectory("utils", AccessLevel::User);
      utils.addCommand<TestCommand>("tool1", AccessLevel::User);
      utils.addCommand<TestCommand>("tool2", AccessLevel::User);
      utils.addDirectory("common", AccessLevel::User);
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
      
      auto& folder1 = root->addDirectory("folder1", AccessLevel::User);
      auto& subfolder1 = folder1.addDirectory("subfolder1", AccessLevel::User);
      subfolder1.addDirectory("deep", AccessLevel::User);
      folder1.addDirectory("subfolder2", AccessLevel::User);
      
      auto& folder2 = root->addDirectory("folder2", AccessLevel::User);
      auto& target1 = folder2.addDirectory("target1", AccessLevel::User);
      target1.addCommand<TestCommand>("item", AccessLevel::User);
      
      auto& target2 = folder2.addDirectory("target2", AccessLevel::User);
      target2.addCommand<TestCommand>("item1", AccessLevel::User);
      target2.addCommand<TestCommand>("item2", AccessLevel::User);

      // Keep reference to test directory for relative path testing
      testDir = &subfolder1;
    }

    std::unique_ptr<Directory> root;
    Directory* testDir;  // Reference to /folder1/subfolder1
  };

  TEST_F(ParentPathCompletionTest, DISABLED_SimpleParentNavigation)
  {
    // From /folder1/subfolder1, completing "../sub" should match subfolder2
    auto result = PathCompleter::complete(*testDir, "../sub", AccessLevel::User);

    EXPECT_EQ(result.matchedNode, "subfolder2");
    EXPECT_EQ(result.fillCharacters, "folder2");
    EXPECT_TRUE(result.isDirectory);
  }

  TEST_F(ParentPathCompletionTest, DISABLED_DoubleParentNavigation)
  {
    // From /folder1/subfolder1, completing "../../f" should show both folder1 and folder2
    auto result = PathCompleter::complete(*testDir, "../../f", AccessLevel::User);

    ASSERT_EQ(result.allOptions.size(), 2);
    EXPECT_EQ(result.allOptions[0], "folder1/");
    EXPECT_EQ(result.allOptions[1], "folder2/");
    EXPECT_EQ(result.matchedNode, "folder");
    EXPECT_EQ(result.fillCharacters, "older");
  }

  TEST_F(ParentPathCompletionTest, DISABLED_ParentWithDeepPath)
  {
    // From /folder1/subfolder1, completing "../../folder2/target" should show target1 and target2
    auto result = PathCompleter::complete(*testDir, "../../folder2/target", AccessLevel::User);

    ASSERT_EQ(result.allOptions.size(), 2);
    EXPECT_EQ(result.allOptions[0], "target1/");
    EXPECT_EQ(result.allOptions[1], "target2/");
    EXPECT_EQ(result.matchedNode, "target");
    EXPECT_TRUE(result.isDirectory);
  }

  TEST_F(ParentPathCompletionTest, DISABLED_MultipleParentsThenDeep)
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

  TEST_F(ParentPathCompletionTest, DISABLED_ParentPastRoot)
  {
    // Going past root should still work and normalize the path
    auto result = PathCompleter::complete(*testDir, "../../../../../../../folder", AccessLevel::User);

    ASSERT_EQ(result.allOptions.size(), 2);
    EXPECT_EQ(result.allOptions[0], "folder1/");
    EXPECT_EQ(result.allOptions[1], "folder2/");
  }

  TEST_F(ParentPathCompletionTest, DISABLED_MixedRelativeAndParentPaths)
  {
    // Test a complex path mixing ./ and ../
    auto result = PathCompleter::complete(*testDir, "./../subfolder2/../subfolder1/d", AccessLevel::User);

    ASSERT_EQ(result.allOptions.size(), 1);
    EXPECT_EQ(result.allOptions[0], "deep/");
    EXPECT_EQ(result.matchedNode, "deep");
  }

  TEST_F(ParentPathCompletionTest, DISABLED_CompletionAfterExactParentMatch)
  {
    // Test completion when the path up to the last segment is exact
    auto result = PathCompleter::complete(*testDir, "../../folder2/target2/", AccessLevel::User);

    ASSERT_EQ(result.allOptions.size(), 2);
    EXPECT_EQ(result.allOptions[0], "item1");
    EXPECT_EQ(result.allOptions[1], "item2");
  }

  TEST_F(ParentPathCompletionTest, DISABLED_NonexistentParentPath)
  {
    // Test completion with a nonexistent path containing parent references
    auto result = PathCompleter::complete(*testDir, "../nonexistent/../sub", AccessLevel::User);

    EXPECT_TRUE(result.allOptions.empty());
    EXPECT_TRUE(result.fillCharacters.empty());
  }

  TEST_F(ParentPathCompletionTest, DISABLED_ParentNavigationWithSimilarNames)
  {
    // Add some directories with similar names to test precise matching
    auto& folder1 = static_cast<Directory&>(*root->findNode({"folder1"}));
    folder1.addDirectory("similar", AccessLevel::User);
    folder1.addDirectory("similar2", AccessLevel::User);

    auto result = PathCompleter::complete(*testDir, "../sim", AccessLevel::User);

    ASSERT_EQ(result.allOptions.size(), 2);
    EXPECT_EQ(result.allOptions[0], "similar/");
    EXPECT_EQ(result.allOptions[1], "similar2/");
    EXPECT_EQ(result.matchedNode, "similar");
  }

}
