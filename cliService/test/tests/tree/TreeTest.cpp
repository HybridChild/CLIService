#include <gtest/gtest.h>
#include "cliService/tree/Directory.hpp"
#include "cliService/tree/CommandResponse.hpp"

namespace cliService
{
  enum class AccessLevel
  {
    User,
    Admin
  };

  // Test command that tracks execution
  class TestCommand : public CommandIf
  {
  public:
    TestCommand(std::string name, AccessLevel level, std::string description = "")
      : CommandIf(std::move(name), level, std::move(description))
      , _wasExecuted(false)
      , _lastArgs()
    {}

    CommandResponse execute(const std::vector<std::string>& args) override
    {
      _wasExecuted = true;
      _lastArgs = args;
      return CommandResponse::success();
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
    Directory& dir = _root->addDynamicDirectory("test", AccessLevel::User);
    EXPECT_EQ(dir.getName(), "test");
    EXPECT_TRUE(dir.isDirectory());
    EXPECT_EQ(dir.getParent(), _root.get());
  }

  TEST_F(TreeTest, AddCommand)
  {
    TestCommand& cmd = _root->addDynamicCommand<TestCommand>("test", AccessLevel::Admin);
    EXPECT_EQ(cmd.getName(), "test");
    EXPECT_FALSE(cmd.isDirectory());
    EXPECT_EQ(cmd.getParent(), _root.get());
  }

  TEST_F(TreeTest, FindNodeInRoot)
  {
    auto& cmd = _root->addDynamicCommand<TestCommand>("test", AccessLevel::Admin);

    NodeIf* found = _root->findNode({"test"});
    EXPECT_EQ(found, &cmd);
  }

  TEST_F(TreeTest, FindNodeInSubdirectory)
  {
    auto& dir = _root->addDynamicDirectory("subdir", AccessLevel::User);
    auto& cmd = dir.addDynamicCommand<TestCommand>("test", AccessLevel::Admin);

    NodeIf* found = _root->findNode({"subdir", "test"});
    EXPECT_EQ(found, &cmd);
  }

  TEST_F(TreeTest, FindNodeInDeepStructure)
  {
    auto& level1 = _root->addDynamicDirectory("level1", AccessLevel::User);
    auto& level2 = level1.addDynamicDirectory("level2", AccessLevel::User);
    auto& level3 = level2.addDynamicDirectory("level3", AccessLevel::User);
    auto& cmd = level3.addDynamicCommand<TestCommand>("test", AccessLevel::Admin);

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
    auto& cmd = _root->addDynamicCommand<TestCommand>("test", AccessLevel::Admin);

    // Trying to traverse through a command
    NodeIf* found = _root->findNode({"test", "subpath"});
    EXPECT_EQ(found, nullptr);
  }

  TEST_F(TreeTest, CommandExecution)
  {
    auto& cmd = _root->addDynamicCommand<TestCommand>("test", AccessLevel::Admin);
    
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
    _root->traverse([&visited](const NodeIf& node, size_t depth) {
      visited.push_back(node.getName());
    });

    EXPECT_EQ(visited.size(), 1);
    EXPECT_EQ(visited[0], "root");
  }

  TEST_F(TreeTest, TraverseComplexTree)
  {
    auto& dir1 = _root->addDynamicDirectory("dir1", AccessLevel::User);
    dir1.addDynamicCommand<TestCommand>("test", AccessLevel::Admin);
    auto& dir2 = dir1.addDynamicDirectory("dir2", AccessLevel::User);
    dir2.addDynamicCommand<TestCommand>("test", AccessLevel::Admin);

    std::vector<std::pair<std::string, int>> visited;
    _root->traverse([&visited](const NodeIf& node, size_t depth) {
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
    _root->addDynamicCommand<TestCommand>("test", AccessLevel::Admin);
    EXPECT_DEATH(_root->addDynamicCommand<TestCommand>("test", AccessLevel::Admin), "");
  }

  TEST_F(TreeTest, NameCollisionInSubdirectory)
  {
    auto& dir = _root->addDynamicDirectory("subdir", AccessLevel::User);
    dir.addDynamicCommand<TestCommand>("test", AccessLevel::Admin);
    EXPECT_DEATH(dir.addDynamicCommand<TestCommand>("test", AccessLevel::Admin), "");
  }

  TEST_F(TreeTest, DirectoryNameCollision)
  {
    _root->addDynamicDirectory("test", AccessLevel::User);
    EXPECT_DEATH(_root->addDynamicDirectory("test", AccessLevel::User), "");
  }

  TEST_F(TreeTest, FindEmptyPath)
  {
    NodeIf* found = _root->findNode({});
    EXPECT_EQ(found, _root.get());
  }

  TEST_F(TreeTest, MixedNameCollision)
  {
    _root->addDynamicDirectory("test", AccessLevel::User);
    EXPECT_DEATH(_root->addDynamicCommand<TestCommand>("test", AccessLevel::Admin), "");
  }

  // Path Resolution Tests
  TEST_F(TreeTest, ResolveAbsolutePath)
  {
    auto& dir = _root->addDynamicDirectory("test", AccessLevel::User);
    auto& cmd = dir.addDynamicCommand<TestCommand>("cmd", AccessLevel::Admin);

    NodeIf* found = _root->resolvePath("/test/cmd", *_root);
    EXPECT_EQ(found, &cmd);
  }

  TEST_F(TreeTest, ResolveRelativePath)
  {
    auto& dir1 = _root->addDynamicDirectory("dir1", AccessLevel::User);
    auto& dir2 = dir1.addDynamicDirectory("dir2", AccessLevel::User);
    auto& cmd = dir2.addDynamicCommand<TestCommand>("cmd", AccessLevel::Admin);

    // Test relative path from dir1
    NodeIf* found = dir1.resolvePath("dir2/cmd", dir1);
    EXPECT_EQ(found, &cmd);

    // Test relative path with parent directory
    found = dir2.resolvePath("../dir2/cmd", dir2);
    EXPECT_EQ(found, &cmd);
  }

  TEST_F(TreeTest, ResolvePathWithDots)
  {
    auto& dir = _root->addDynamicDirectory("test", AccessLevel::User);
    auto& cmd = dir.addDynamicCommand<TestCommand>("cmd", AccessLevel::Admin);

    NodeIf* found = _root->resolvePath("/test/./cmd", *_root);
    EXPECT_EQ(found, &cmd);

    found = _root->resolvePath("/test/../test/cmd", *_root);
    EXPECT_EQ(found, &cmd);
  }

  // Relative Path Generation Tests
  TEST_F(TreeTest, GetRelativePathBetweenNodes)
  {
    auto& dir1 = _root->addDynamicDirectory("dir1", AccessLevel::User);
    auto& dir2 = dir1.addDynamicDirectory("dir2", AccessLevel::User);
    auto& cmd = dir2.addDynamicCommand<TestCommand>("cmd", AccessLevel::Admin);

    Path relativePath = dir1.getRelativePath(cmd);
    EXPECT_EQ(relativePath.toString(), "dir2/cmd");

    relativePath = dir2.getRelativePath(dir1);
    EXPECT_EQ(relativePath.toString(), "..");
  }

  // Access Level Tests
  TEST_F(TreeTest, NodesRetainAccessLevels)
  {
    auto& userDir = _root->addDynamicDirectory("user", AccessLevel::User);
    auto& adminDir = _root->addDynamicDirectory("admin", AccessLevel::Admin);
    auto& userCmd = userDir.addDynamicCommand<TestCommand>("cmd", AccessLevel::User);
    auto& adminCmd = adminDir.addDynamicCommand<TestCommand>("cmd", AccessLevel::Admin);

    EXPECT_EQ(userDir.getAccessLevel(), AccessLevel::User);
    EXPECT_EQ(adminDir.getAccessLevel(), AccessLevel::Admin);
    EXPECT_EQ(userCmd.getAccessLevel(), AccessLevel::User);
    EXPECT_EQ(adminCmd.getAccessLevel(), AccessLevel::Admin);
  }

  // Edge Cases and Error Handling
  TEST_F(TreeTest, ResolveMalformedPaths)
  {
    auto& dir = _root->addDynamicDirectory("test", AccessLevel::User);
    
    // Extra slashes
    NodeIf* found = _root->resolvePath("//test//", *_root);
    EXPECT_EQ(found, &dir);

    // Dots without slashes
    // These should resolve to the same node
    found = _root->resolvePath("test", *_root);         // Basic path
    EXPECT_EQ(found, &dir);
    found = _root->resolvePath("test/.", *_root);       // With explicit current dir
    EXPECT_EQ(found, &dir);
    found = _root->resolvePath("./test", *_root);       // From current dir
    EXPECT_EQ(found, &dir);
    found = _root->resolvePath("./test/.", *_root);     // Combined
    EXPECT_EQ(found, &dir);

    // These should NOT resolve to the same node
    found = _root->resolvePath("test./.", *_root);      // Should fail - looking for "test."
    EXPECT_NE(found, &dir);
    found = _root->resolvePath("test.", *_root);        // Should fail - looking for "test."
    EXPECT_NE(found, &dir);

    // Empty path components
    found = _root->resolvePath("test//cmd", *_root);
    EXPECT_EQ(found, nullptr);
  }

  TEST_F(TreeTest, NavigateOutOfTreeRoot)
  {
    auto& dir = _root->addDynamicDirectory("test", AccessLevel::User);

    // Try to navigate above root
    NodeIf* found = _root->resolvePath("/test/../../test", *_root);
    EXPECT_EQ(found, &dir);
  }

  // Path Traversal Tests
  TEST_F(TreeTest, TraverseWithFilter)
  {
    auto& dir1 = _root->addDynamicDirectory("dir1", AccessLevel::User);
    dir1.addDynamicCommand<TestCommand>("cmd1", AccessLevel::User);
    auto& dir2 = dir1.addDynamicDirectory("dir2", AccessLevel::Admin);
    dir2.addDynamicCommand<TestCommand>("cmd2", AccessLevel::Admin);

    // Count only Admin level nodes
    int adminCount = 0;
    _root->traverse([&adminCount](const NodeIf& node, size_t depth) {
      if (node.getAccessLevel() == AccessLevel::Admin) {
        adminCount++;
      }
    });

    EXPECT_EQ(adminCount, 2); // dir2 and cmd2
  }

  TEST_F(TreeTest, TraverseMaxDepth)
  {
    auto& dir1 = _root->addDynamicDirectory("dir1", AccessLevel::User);
    auto& dir2 = dir1.addDynamicDirectory("dir2", AccessLevel::User);
    auto& dir3 = dir2.addDynamicDirectory("dir3", AccessLevel::User);
    dir3.addDynamicCommand<TestCommand>("cmd", AccessLevel::User);

    // Count nodes at different depths
    std::vector<int> nodesAtDepth(5, 0);
    _root->traverse([&nodesAtDepth](const NodeIf& node, size_t depth) {
      if (depth < nodesAtDepth.size()) {
        nodesAtDepth[depth]++;
      }
    });

    EXPECT_EQ(nodesAtDepth[0], 1); // root
    EXPECT_EQ(nodesAtDepth[1], 1); // dir1
    EXPECT_EQ(nodesAtDepth[2], 1); // dir2
    EXPECT_EQ(nodesAtDepth[3], 1); // dir3
    EXPECT_EQ(nodesAtDepth[4], 1); // cmd
  }

  // Command Tests
  TEST_F(TreeTest, CommandWithDescription)
  {
    auto& cmd = _root->addDynamicCommand<TestCommand>(
      "test", 
      AccessLevel::Admin,
      "Test command description"
    );

    EXPECT_EQ(cmd.getDescription(), "Test command description");
  }

  TEST_F(TreeTest, CommandStatusResponses)
  {
    class StatusTestCommand : public CommandIf
    {
    public:
      using CommandIf::CommandIf;
      
      CommandResponse execute(const std::vector<std::string>& args) override
      {
        if (args.empty()) {
          return CommandResponse::error("No arguments provided");
        }
        if (args[0] == "invalid") {
          return CommandIf::createInvalidArgumentCountResponse(2);
        }
        return CommandResponse::success("Command executed successfully");
      }
    };

    auto& cmd = _root->addDynamicCommand<StatusTestCommand>("test", AccessLevel::User);

    auto response = cmd.execute({});
    EXPECT_EQ(response.getStatus(), CommandStatus::Error);
    EXPECT_EQ(response.getMessage(), "No arguments provided");

    response = cmd.execute({"invalid"});
    EXPECT_EQ(response.getStatus(), CommandStatus::InvalidArguments);

    response = cmd.execute({"valid"});
    EXPECT_EQ(response.getStatus(), CommandStatus::Success);
    EXPECT_EQ(response.getMessage(), "Command executed successfully");
  }


class StaticTreeTest : public ::testing::Test
{
protected:
  StaticTreeTest()
    : root("root", AccessLevel::User)
    , sysDir("system", AccessLevel::Admin)
    , hwDir("hw", AccessLevel::User)
    , rebootCmd("reboot", AccessLevel::Admin)
    , heapCmd("heap", AccessLevel::Admin)
  {}

  void SetUp() override
  {
    // Build tree
    root.addStaticDirectory(sysDir);
    root.addStaticDirectory(hwDir);
    sysDir.addStaticCommand(rebootCmd);
    sysDir.addStaticCommand(heapCmd);
  }

  Directory root;
  Directory sysDir;
  Directory hwDir;
  TestCommand rebootCmd;
  TestCommand heapCmd;
};


  TEST_F(StaticTreeTest, StaticTreeStructure)
  {
    NodeIf* foundSys = root.findNode({"system"});
    ASSERT_NE(foundSys, nullptr);
    EXPECT_EQ(foundSys, &sysDir);
    
    NodeIf* foundHw = root.findNode({"hw"});
    ASSERT_NE(foundHw, nullptr);
    EXPECT_EQ(foundHw, &hwDir);
    
    NodeIf* foundReboot = root.findNode({"system", "reboot"});
    ASSERT_NE(foundReboot, nullptr);
    EXPECT_EQ(foundReboot, &rebootCmd);
  }

  TEST_F(StaticTreeTest, StaticCommandExecution)
  {
    NodeIf* found = root.findNode({"system", "reboot"});
    ASSERT_NE(found, nullptr);
    
    auto* cmd = dynamic_cast<TestCommand*>(found);
    ASSERT_NE(cmd, nullptr);
    
    std::vector<std::string> args = {"test_arg"};
    cmd->execute(args);
    
    EXPECT_TRUE(cmd->wasExecuted());
    EXPECT_EQ(cmd->getLastArgs(), args);
  }

  TEST_F(StaticTreeTest, StaticTraversal)
  {
    std::vector<std::pair<std::string, int>> visited;
    root.traverse([&visited](const NodeIf& node, size_t depth) {
      visited.push_back({node.getName(), depth});
    });

    ASSERT_EQ(visited.size(), 5);
    EXPECT_EQ(visited[0], std::make_pair(std::string("root"), 0));
    EXPECT_EQ(visited[1], std::make_pair(std::string("system"), 1));
    EXPECT_EQ(visited[2], std::make_pair(std::string("reboot"), 2));
    EXPECT_EQ(visited[3], std::make_pair(std::string("heap"), 2));
    EXPECT_EQ(visited[4], std::make_pair(std::string("hw"), 1));
  }

  // Tests for mixed static/dynamic allocation
  TEST_F(TreeTest, MixStaticAndDynamicNodes)
  {
    // Add static directory to dynamic root
    Directory staticDir("static", AccessLevel::User);
    TestCommand staticCmd("cmd", AccessLevel::Admin);
    
    _root->addStaticDirectory(staticDir);
    staticDir.addStaticCommand(staticCmd);
    
    // Add dynamic directory to static directory
    auto& dynamicDir = staticDir.addDynamicDirectory("dynamic", AccessLevel::User);
    auto& dynamicCmd = dynamicDir.addDynamicCommand<TestCommand>("test", AccessLevel::Admin);
    
    // Verify structure
    EXPECT_NE(_root->findNode({"static"}), nullptr);
    EXPECT_NE(_root->findNode({"static", "cmd"}), nullptr);
    EXPECT_NE(_root->findNode({"static", "dynamic"}), nullptr);
    EXPECT_NE(_root->findNode({"static", "dynamic", "test"}), nullptr);
    
    // Verify command execution
    auto* cmd = dynamic_cast<TestCommand*>(_root->findNode({"static", "dynamic", "test"}));
    ASSERT_NE(cmd, nullptr);
    
    cmd->execute({"test"});
    EXPECT_TRUE(cmd->wasExecuted());
  }

  TEST_F(TreeTest, StaticNodeNameCollision)
  {
    Directory staticDir("test", AccessLevel::User);
    _root->addStaticDirectory(staticDir);
    
    // Try to add dynamic node with same name
    EXPECT_DEATH(_root->addDynamicDirectory("test", AccessLevel::User), "");
    
    // Try to add another static node with same name
    Directory anotherDir("test", AccessLevel::User);
    EXPECT_DEATH(_root->addStaticDirectory(anotherDir), "");
  }

  TEST_F(TreeTest, DynamicToStaticNodeNameCollision)
  {
    _root->addDynamicDirectory("test", AccessLevel::User);
    
    // Try to add static node with same name
    Directory staticDir("test", AccessLevel::User);
    EXPECT_DEATH(_root->addStaticDirectory(staticDir), "");
  }

}
