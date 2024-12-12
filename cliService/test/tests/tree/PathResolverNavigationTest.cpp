#include <gtest/gtest.h>
#include "cliService/tree/PathResolver.hpp"

namespace cliService
{
  enum class AccessLevel
  {
    Public
  };

  class PathResolverNavigationTest : public ::testing::Test
  {
  protected:
    void SetUp() override
    {
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
      subdir2 = &dir1.addDirectory("subdir2", AccessLevel::Public);
      
      auto& dir2 = root->addDirectory("dir2", AccessLevel::Public);
      dir2.addDirectory("subdir3", AccessLevel::Public);
      
      resolver = std::make_unique<PathResolver>(*root);
    }

    std::unique_ptr<Directory> root;
    Directory* subdir2;  // Keep track of this for testing
    std::unique_ptr<PathResolver> resolver;
  };

  TEST_F(PathResolverNavigationTest, ParentNavigationFromNested)
  {
    // From /dir1/subdir2, go up two levels and to dir2
    auto* node = resolver->resolveFromString("../../dir2", *subdir2);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->getName(), "dir2");
  }

  TEST_F(PathResolverNavigationTest, MultipleParentNavigations)
  {
    // Try going up beyond root
    auto* node = resolver->resolveFromString("../../../../dir2", *subdir2);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->getName(), "dir2");
  }

  TEST_F(PathResolverNavigationTest, MixedNavigationWithParents)
  {
    // Complex path with parent refs mixed in
    auto* node = resolver->resolveFromString("../subdir1/../subdir2/../../dir2/subdir3", *subdir2);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->getName(), "subdir3");
  }

  TEST_F(PathResolverNavigationTest, AbsolutePathWithParents)
  {
    // Absolute path with parent references
    auto* node = resolver->resolveFromString("/dir1/subdir2/../../dir2", *subdir2);
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->getName(), "dir2");
  }

}
