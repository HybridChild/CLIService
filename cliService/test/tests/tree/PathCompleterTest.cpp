#include <gtest/gtest.h>
#include <memory>
#include "cliService/tree/PathCompleter.hpp"

namespace cliService
{
  enum class AccessLevel { User, Admin };
}

using namespace cliService;

class TestCommand : public CommandIf
{
public:
  TestCommand(std::string name, AccessLevel level, std::string description = "") 
    : CommandIf(std::move(name), level, std::move(description))
  {}
  
  CommandResponse execute(const std::vector<std::string>&) override
  {
    return CommandResponse::success();
  }
};

class PathCompleterTest : public ::testing::Test
{
protected:
  void SetUp() override {
    root = std::make_unique<Directory>("root", AccessLevel::User);
    
    // /utils/
    auto& utils = root->addDirectory("utils", AccessLevel::User);
    utils.addCommand<TestCommand>("print", AccessLevel::User);
    utils.addCommand<TestCommand>("help", AccessLevel::User);
    
    // /utils/format/
    auto& format = utils.addDirectory("format", AccessLevel::User);
    format.addCommand<TestCommand>("json", AccessLevel::User);
    format.addCommand<TestCommand>("xml", AccessLevel::User);
    
    // /admin/ (restricted access)
    auto& admin = root->addDirectory("admin", AccessLevel::Admin);
    admin.addCommand<TestCommand>("config", AccessLevel::Admin);
    admin.addCommand<TestCommand>("users", AccessLevel::Admin);
  }

  std::unique_ptr<Directory> root;
};

TEST_F(PathCompleterTest, EmptyInput)
{
  auto result = PathCompleter::complete(*root, "", AccessLevel::User);
  
  ASSERT_FALSE(result.allOptions.empty());
  ASSERT_EQ(result.allOptions.size(), 1);  // only utils/ visible to user
  EXPECT_EQ(result.allOptions[0], "utils/");
  EXPECT_TRUE(result.fillCharacters.empty());
}

TEST_F(PathCompleterTest, SimpleCompletion)
{
  auto result = PathCompleter::complete(*root, "ut", AccessLevel::User);
  
  EXPECT_EQ(result.fullPath, "utils");
  EXPECT_EQ(result.matchedNode, "utils");
  EXPECT_EQ(result.fillCharacters, "ils");
  EXPECT_TRUE(result.isDirectory);
  ASSERT_EQ(result.allOptions.size(), 1);
  EXPECT_EQ(result.allOptions[0], "utils/");
}

TEST_F(PathCompleterTest, PartialCompletion)
{
  auto result = PathCompleter::complete(*root, "utils/p", AccessLevel::User);
  
  EXPECT_EQ(result.fullPath, "utils/print");
  EXPECT_EQ(result.matchedNode, "print");
  EXPECT_EQ(result.fillCharacters, "rint");
  EXPECT_FALSE(result.isDirectory);
  ASSERT_EQ(result.allOptions.size(), 1);
  EXPECT_EQ(result.allOptions[0], "print");
}

TEST_F(PathCompleterTest, DirectoryCompletion)
{
  auto result = PathCompleter::complete(*root, "utils/f", AccessLevel::User);
  
  EXPECT_EQ(result.fullPath, "utils/format");
  EXPECT_EQ(result.matchedNode, "format");
  EXPECT_EQ(result.fillCharacters, "ormat");
  EXPECT_TRUE(result.isDirectory);
  ASSERT_EQ(result.allOptions.size(), 1);
}

TEST_F(PathCompleterTest, NestedPathCompletion)
{
  auto result = PathCompleter::complete(*root, "utils/format/", AccessLevel::User);
  
  ASSERT_EQ(result.allOptions.size(), 2);
  EXPECT_EQ(result.allOptions[0], "json");
  EXPECT_EQ(result.allOptions[1], "xml");
  EXPECT_TRUE(result.fillCharacters.empty());
}

TEST_F(PathCompleterTest, AccessLevelRestrictions)
{
  // Test as regular user
  {
    auto result = PathCompleter::complete(*root, "ad", AccessLevel::User);
    EXPECT_TRUE(result.allOptions.empty());
    EXPECT_TRUE(result.fillCharacters.empty());
  }
  
  // Test as admin
  {
    auto result = PathCompleter::complete(*root, "ad", AccessLevel::Admin);
    ASSERT_FALSE(result.allOptions.empty());
    EXPECT_EQ(result.fullPath, "admin");
    EXPECT_EQ(result.fillCharacters, "min");
  }
}

TEST_F(PathCompleterTest, MultipleMatches)
{
  // Add another command starting with 'p' to test multiple matches
  static_cast<Directory*>(root->findNode({"utils"}))->addCommand<TestCommand>("process", AccessLevel::User);
  
  auto result = PathCompleter::complete(*root, "utils/p", AccessLevel::User);
  
  ASSERT_EQ(result.allOptions.size(), 2);
  ASSERT_EQ(result.allOptions[0], "print");
  ASSERT_EQ(result.allOptions[1], "process");
  EXPECT_EQ(result.matchedNode, "pr");
  EXPECT_EQ(result.fillCharacters, "r");
}

TEST_F(PathCompleterTest, NoMatches)
{
  auto result = PathCompleter::complete(*root, "nonexistent", AccessLevel::User);
  
  EXPECT_TRUE(result.allOptions.empty());
  EXPECT_TRUE(result.fullPath.empty());
  EXPECT_TRUE(result.matchedNode.empty());
  EXPECT_TRUE(result.fillCharacters.empty());
}

TEST_F(PathCompleterTest, AbsolutePaths)
{
  auto result = PathCompleter::complete(*root, "/utils/format/j", AccessLevel::User);
  
  EXPECT_EQ(result.fullPath, "/utils/format/json");
  EXPECT_EQ(result.matchedNode, "json");
  EXPECT_EQ(result.fillCharacters, "son");
  EXPECT_FALSE(result.isDirectory);
}

TEST_F(PathCompleterTest, EdgeCases)
{
  // Double slashes
  {
    auto result = PathCompleter::complete(*root, "utils//format//", AccessLevel::User);
    EXPECT_FALSE(result.allOptions.empty());
    EXPECT_TRUE(result.fillCharacters.empty());
  }
  
  // Trailing slash
  {
    auto result = PathCompleter::complete(*root, "utils/", AccessLevel::User);
    EXPECT_FALSE(result.allOptions.empty());
    EXPECT_TRUE(result.fillCharacters.empty());
  }
  
  // Path with no completable part
  {
    auto result = PathCompleter::complete(*root, "utils/format/json/", AccessLevel::User);
    EXPECT_TRUE(result.allOptions.empty());
    EXPECT_TRUE(result.fillCharacters.empty());
  }
}

TEST_F(PathCompleterTest, CompletionWithExactMatch)
{
  auto result = PathCompleter::complete(*root, "utils", AccessLevel::User);
  
  EXPECT_EQ(result.fullPath, "utils");
  EXPECT_EQ(result.matchedNode, "utils");
  EXPECT_TRUE(result.fillCharacters.empty());
  EXPECT_TRUE(result.isDirectory);
}

TEST_F(PathCompleterTest, NestedCompletionWithPartialMatch)
{
  auto result = PathCompleter::complete(*root, "utils/format/j", AccessLevel::User);
  
  EXPECT_EQ(result.fullPath, "utils/format/json");
  EXPECT_EQ(result.matchedNode, "json");
  EXPECT_EQ(result.fillCharacters, "son");
  EXPECT_FALSE(result.isDirectory);
}
