// PathResolverTest.cpp
#include "cliService/tree/PathResolver.hpp"
#include "cliService/tree/Directory.hpp"
#include <gtest/gtest.h>

namespace cliService
{
  enum class AccessLevel {
    Public,
    Private
  };
}

using namespace cliService;


class PathResolverTest : public ::testing::Test
{
protected:
  void SetUp() override {
    // Create a test directory structure:
    // /
    // ├── dir1
    // │   ├── subdir1
    // │   └── subdir2
    // └── dir2
    //     └── subdir3
    
    root = std::make_unique<Directory>("", AccessLevel::Public);
    auto& dir1 = root->addDirectory("dir1", AccessLevel::Public);
    dir1.addDirectory("subdir1", AccessLevel::Public);
    dir1.addDirectory("subdir2", AccessLevel::Public);
    
    auto& dir2 = root->addDirectory("dir2", AccessLevel::Public);
    dir2.addDirectory("subdir3", AccessLevel::Public);
    
    resolver = std::make_unique<PathResolver>(*root);
  }

  std::unique_ptr<Directory> root;
  std::unique_ptr<PathResolver> resolver;
};

TEST_F(PathResolverTest, ResolveRoot)
{
  Path path("/");
  auto* node = resolver->resolve(path, *root);
  EXPECT_EQ(node, root.get());
}

TEST_F(PathResolverTest, ResolveAbsolutePath)
{
  Path path("/dir1/subdir1");
  auto* node = resolver->resolve(path, *root);
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->getName(), "subdir1");
}

TEST_F(PathResolverTest, ResolveRelativePath)
{
  // First navigate to dir1
  Path dir1Path("/dir1");
  auto* dir1 = resolver->resolve(dir1Path, *root);
  ASSERT_NE(dir1, nullptr);
  
  // Then resolve relative path from dir1
  Path path("subdir2");
  auto* node = resolver->resolve(path, *static_cast<Directory*>(dir1));
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->getName(), "subdir2");
}

TEST_F(PathResolverTest, ResolveParentDirectory)
{
  // First navigate to subdir1
  Path subdir1Path("/dir1/subdir1");
  auto* subdir1 = resolver->resolve(subdir1Path, *root);
  ASSERT_NE(subdir1, nullptr);
  
  // Then go up one level
  Path path("..");
  auto* node = resolver->resolve(path, *static_cast<Directory*>(subdir1));
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->getName(), "dir1");
}

TEST_F(PathResolverTest, ResolveNonexistentPath)
{
  Path path("/nonexistent/path");
  auto* node = resolver->resolve(path, *root);
  EXPECT_EQ(node, nullptr);
}

TEST_F(PathResolverTest, ResolveComplexPath)
{
  Path path("/dir1/subdir1/../../dir2/subdir3");
  auto* node = resolver->resolve(path, *root);
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->getName(), "subdir3");
}

TEST_F(PathResolverTest, GetAbsolutePath)
{
  // First get to subdir3
  Path path("/dir2/subdir3");
  auto* node = resolver->resolve(path, *root);
  ASSERT_NE(node, nullptr);
  
  // Get its absolute path
  Path absPath = PathResolver::getAbsolutePath(*node);
  EXPECT_TRUE(absPath.isAbsolute());
  ASSERT_EQ(absPath.elements().size(), 2);
  EXPECT_EQ(absPath.elements()[0], "dir2");
  EXPECT_EQ(absPath.elements()[1], "subdir3");
}

TEST_F(PathResolverTest, ResolveEmptyPath)
{
  Path path("");
  auto* node = resolver->resolve(path, *root);
  EXPECT_EQ(node, root.get());
}

TEST_F(PathResolverTest, ResolveDotPath)
{
  // First navigate to dir1
  Path dir1Path("/dir1");
  auto* dir1 = resolver->resolve(dir1Path, *root);
  ASSERT_NE(dir1, nullptr);
  
  // Resolve "." from dir1
  Path path(".");
  auto* node = resolver->resolve(path, *static_cast<Directory*>(dir1));
  EXPECT_EQ(node, dir1);
}
