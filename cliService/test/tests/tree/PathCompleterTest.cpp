#include <gtest/gtest.h>
#include <memory>
#include "cliService/tree/PathCompleter.hpp"

namespace cliService
{
  enum class AccessLevel { User, Admin };
}

using namespace cliService;

class TestCommand : public CommandIf {
public:
  TestCommand(std::string name, AccessLevel level) 
    : CommandIf(std::move(name), level) {}
  
  CommandResponse execute(const std::vector<std::string>&) override {
    return CommandResponse::success();
  }
};

class PathCompleterTest : public ::testing::Test {
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

TEST_F(PathCompleterTest, EmptyInput) {
  auto result = PathCompleter::complete(*root, "", AccessLevel::User);
  
  ASSERT_FALSE(result.allOptions.empty());
  ASSERT_EQ(result.allOptions.size(), 1);  // only utils/ visible to user
  EXPECT_EQ(result.allOptions[0], "utils/");
  EXPECT_TRUE(result.newCharacters.empty());
}

TEST_F(PathCompleterTest, SimpleCompletion) {
  auto result = PathCompleter::complete(*root, "ut", AccessLevel::User);
  
  EXPECT_EQ(result.fullCompletion, "utils");
  EXPECT_EQ(result.partialCompletion, "utils");
  EXPECT_EQ(result.newCharacters, "ils");
  EXPECT_TRUE(result.isDirectory);
  ASSERT_EQ(result.allOptions.size(), 1);
}

TEST_F(PathCompleterTest, PartialCompletion) {
  auto result = PathCompleter::complete(*root, "utils/p", AccessLevel::User);
  
  EXPECT_EQ(result.fullCompletion, "utils/print");
  EXPECT_EQ(result.partialCompletion, "print");
  EXPECT_EQ(result.newCharacters, "rint");
  EXPECT_FALSE(result.isDirectory);
  ASSERT_EQ(result.allOptions.size(), 1);
}

TEST_F(PathCompleterTest, DirectoryCompletion) {
  auto result = PathCompleter::complete(*root, "utils/f", AccessLevel::User);
  
  EXPECT_EQ(result.fullCompletion, "utils/format");
  EXPECT_EQ(result.partialCompletion, "format");
  EXPECT_EQ(result.newCharacters, "ormat");
  EXPECT_TRUE(result.isDirectory);
  ASSERT_EQ(result.allOptions.size(), 1);
}

TEST_F(PathCompleterTest, NestedPathCompletion) {
  auto result = PathCompleter::complete(*root, "utils/format/", AccessLevel::User);
  
  ASSERT_EQ(result.allOptions.size(), 2);
  EXPECT_EQ(result.allOptions[0], "json");
  EXPECT_EQ(result.allOptions[1], "xml");
  EXPECT_TRUE(result.newCharacters.empty());
}

TEST_F(PathCompleterTest, AccessLevelRestrictions) {
  // Test as regular user
  {
    auto result = PathCompleter::complete(*root, "ad", AccessLevel::User);
    EXPECT_TRUE(result.allOptions.empty());
    EXPECT_TRUE(result.newCharacters.empty());
  }
  
  // Test as admin
  {
    auto result = PathCompleter::complete(*root, "ad", AccessLevel::Admin);
    ASSERT_FALSE(result.allOptions.empty());
    EXPECT_EQ(result.fullCompletion, "admin");
    EXPECT_EQ(result.newCharacters, "min");
  }
}

TEST_F(PathCompleterTest, MultipleMatches) {
  // Add another command starting with 'p' to test multiple matches
  static_cast<Directory*>(root->findNode({"utils"}))->addCommand<TestCommand>("process", AccessLevel::User);
  
  auto result = PathCompleter::complete(*root, "utils/p", AccessLevel::User);
  
  ASSERT_EQ(result.allOptions.size(), 2);
  EXPECT_EQ(result.partialCompletion, "pr");
  EXPECT_EQ(result.newCharacters, "r");
}

TEST_F(PathCompleterTest, NoMatches) {
  auto result = PathCompleter::complete(*root, "nonexistent", AccessLevel::User);
  
  EXPECT_TRUE(result.allOptions.empty());
  EXPECT_TRUE(result.fullCompletion.empty());
  EXPECT_TRUE(result.partialCompletion.empty());
  EXPECT_TRUE(result.newCharacters.empty());
}

TEST_F(PathCompleterTest, AbsolutePaths) {
  auto result = PathCompleter::complete(*root, "/utils/format/j", AccessLevel::User);
  
  EXPECT_EQ(result.fullCompletion, "/utils/format/json");
  EXPECT_EQ(result.partialCompletion, "json");
  EXPECT_EQ(result.newCharacters, "son");
  EXPECT_FALSE(result.isDirectory);
}

TEST_F(PathCompleterTest, EdgeCases) {
  // Double slashes
  {
    auto result = PathCompleter::complete(*root, "utils//format//", AccessLevel::User);
    EXPECT_FALSE(result.allOptions.empty());
    EXPECT_TRUE(result.newCharacters.empty());
  }
  
  // Trailing slash
  {
    auto result = PathCompleter::complete(*root, "utils/", AccessLevel::User);
    EXPECT_FALSE(result.allOptions.empty());
    EXPECT_TRUE(result.newCharacters.empty());
  }
  
  // Path with no completable part
  {
    auto result = PathCompleter::complete(*root, "utils/format/json/", AccessLevel::User);
    EXPECT_TRUE(result.allOptions.empty());
    EXPECT_TRUE(result.newCharacters.empty());
  }
}

TEST_F(PathCompleterTest, CompletionWithExactMatch) {
  auto result = PathCompleter::complete(*root, "utils", AccessLevel::User);
  
  EXPECT_EQ(result.fullCompletion, "utils");
  EXPECT_EQ(result.partialCompletion, "utils");
  EXPECT_TRUE(result.newCharacters.empty());
  EXPECT_TRUE(result.isDirectory);
}

TEST_F(PathCompleterTest, NestedCompletionWithPartialMatch) {
  auto result = PathCompleter::complete(*root, "utils/format/j", AccessLevel::User);
  
  EXPECT_EQ(result.fullCompletion, "utils/format/json");
  EXPECT_EQ(result.partialCompletion, "json");
  EXPECT_EQ(result.newCharacters, "son");
  EXPECT_FALSE(result.isDirectory);
}
