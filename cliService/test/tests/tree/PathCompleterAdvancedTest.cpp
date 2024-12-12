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

}
