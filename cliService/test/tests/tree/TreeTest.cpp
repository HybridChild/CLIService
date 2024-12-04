#include <gtest/gtest.h>
#include "cliService/tree/Directory.hpp"

namespace cliService
{
  enum class AccessLevel
  {
    User,
    Admin
  };
}

using namespace cliService;

// Test command that tracks execution
class TestCommand : public CommandIf
{
public:
  TestCommand(std::string name, AccessLevel level)
    : CommandIf(name, level)
    , _wasExecuted(false)
    , _lastArgs()
  {}

  void execute(const std::vector<std::string>& args) override
  {
    _wasExecuted = true;
    _lastArgs = args;
  }

  bool wasExecuted() const { return _wasExecuted; }
  const std::vector<std::string>& getLastArgs() const { return _lastArgs; }

private:
  bool _wasExecuted;
  std::vector<std::string> _lastArgs;
};

class TreeTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    _root = std::make_unique<Directory>("root", AccessLevel::User);
  }

  std::unique_ptr<Directory> _root;
};

TEST_F(TreeTest, RootDirectoryHasCorrectName)
{
  EXPECT_EQ(_root->getName(), "root");
  EXPECT_TRUE(_root->isDirectory());
}

TEST_F(TreeTest, AddDirectory)
{
  Directory& dir = _root->addDirectory("test", AccessLevel::User);
  EXPECT_EQ(dir.getName(), "test");
  EXPECT_TRUE(dir.isDirectory());
  EXPECT_EQ(dir.getParent(), _root.get());
}

TEST_F(TreeTest, AddCommand)
{
  TestCommand& cmd = _root->addCommand<TestCommand>("test", AccessLevel::Admin);
  EXPECT_EQ(cmd.getName(), "test");
  EXPECT_FALSE(cmd.isDirectory());
  EXPECT_EQ(cmd.getParent(), _root.get());
}

TEST_F(TreeTest, FindNodeInRoot)
{
  auto& cmd = _root->addCommand<TestCommand>("test", AccessLevel::Admin);
  
  NodeIf* found = _root->findNode({"test"});
  EXPECT_EQ(found, &cmd);
}

TEST_F(TreeTest, FindNodeInSubdirectory)
{
  auto& dir = _root->addDirectory("subdir", AccessLevel::User);
  auto& cmd = dir.addCommand<TestCommand>("test", AccessLevel::Admin);
  
  NodeIf* found = _root->findNode({"subdir", "test"});
  EXPECT_EQ(found, &cmd);
}

TEST_F(TreeTest, FindNodeInDeepStructure)
{
  auto& level1 = _root->addDirectory("level1", AccessLevel::User);
  auto& level2 = level1.addDirectory("level2", AccessLevel::User);
  auto& level3 = level2.addDirectory("level3", AccessLevel::User);
  auto& cmd = level3.addCommand<TestCommand>("test", AccessLevel::Admin);
  
  NodeIf* found = _root->findNode({"level1", "level2", "level3", "test"});
  EXPECT_EQ(found, &cmd);
}

TEST_F(TreeTest, FindNonExistentNode)
{
  NodeIf* found = _root->findNode({"nonexistent"});
  EXPECT_EQ(found, nullptr);
}

TEST_F(TreeTest, FindWithInvalidPath)
{
  auto& cmd = _root->addCommand<TestCommand>("test", AccessLevel::Admin);
  
  // Trying to traverse through a command
  NodeIf* found = _root->findNode({"test", "subpath"});
  EXPECT_EQ(found, nullptr);
}

TEST_F(TreeTest, CommandExecution)
{
  auto& cmd = _root->addCommand<TestCommand>("test", AccessLevel::Admin);
  
  std::vector<std::string> args = {"arg1", "arg2"};
  NodeIf* found = _root->findNode({"test"});
  auto* testCmd = dynamic_cast<TestCommand*>(found);
  ASSERT_NE(testCmd, nullptr);
  
  testCmd->execute(args);
  EXPECT_TRUE(testCmd->wasExecuted());
  EXPECT_EQ(testCmd->getLastArgs(), args);
}

TEST_F(TreeTest, TraverseEmptyTree)
{
  std::vector<std::string> visited;
  _root->traverse([&visited](const NodeIf& node, int depth) {
    visited.push_back(node.getName());
  });
  
  EXPECT_EQ(visited.size(), 1);
  EXPECT_EQ(visited[0], "root");
}

TEST_F(TreeTest, TraverseComplexTree)
{
  auto& dir1 = _root->addDirectory("dir1", AccessLevel::User);
  dir1.addCommand<TestCommand>("test", AccessLevel::Admin);
  auto& dir2 = dir1.addDirectory("dir2", AccessLevel::User);
  dir2.addCommand<TestCommand>("test", AccessLevel::Admin);
  
  std::vector<std::pair<std::string, int>> visited;
  _root->traverse([&visited](const NodeIf& node, int depth) {
    visited.push_back({node.getName(), depth});
  });
  
  EXPECT_EQ(visited.size(), 5);
  EXPECT_EQ(visited[0], std::make_pair(std::string("root"), 0));
  EXPECT_EQ(visited[1], std::make_pair(std::string("dir1"), 1));
  EXPECT_EQ(visited[2], std::make_pair(std::string("test"), 2));
  EXPECT_EQ(visited[3], std::make_pair(std::string("dir2"), 2));
  EXPECT_EQ(visited[4], std::make_pair(std::string("test"), 3));
}

TEST_F(TreeTest, NameCollisionInDirectory)
{
  _root->addCommand<TestCommand>("test", AccessLevel::Admin);
  EXPECT_DEATH(_root->addCommand<TestCommand>("test", AccessLevel::Admin), "");
}

TEST_F(TreeTest, NameCollisionInSubdirectory)
{
  auto& dir = _root->addDirectory("subdir", AccessLevel::User);
  dir.addCommand<TestCommand>("test", AccessLevel::Admin);
  EXPECT_DEATH(dir.addCommand<TestCommand>("test", AccessLevel::Admin), "");
}

TEST_F(TreeTest, DirectoryNameCollision)
{
  _root->addDirectory("test", AccessLevel::User);
  EXPECT_DEATH(_root->addDirectory("test", AccessLevel::User), "");
}

TEST_F(TreeTest, FindEmptyPath)
{
  NodeIf* found = _root->findNode({});
  EXPECT_EQ(found, _root.get());
}

TEST_F(TreeTest, MixedNameCollision)
{
  _root->addDirectory("test", AccessLevel::User);
  EXPECT_DEATH(_root->addCommand<TestCommand>("test", AccessLevel::Admin), "");
}
