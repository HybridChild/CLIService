#include "cliService/tree/Path.hpp"
#include <gtest/gtest.h>

using namespace cliService;


class PathTest : public ::testing::Test
{
protected:
  void SetUp() override {}
};

TEST_F(PathTest, EmptyPath)
{
  Path path;
  EXPECT_TRUE(path.isEmpty());
  EXPECT_FALSE(path.isAbsolute());
  EXPECT_TRUE(path.components().empty());
}

TEST_F(PathTest, AbsolutePathConstruction)
{
  Path path("/dir1/dir2");
  EXPECT_FALSE(path.isEmpty());
  EXPECT_TRUE(path.isAbsolute());
  ASSERT_EQ(path.components().size(), 2);
  EXPECT_EQ(path.components()[0], "dir1");
  EXPECT_EQ(path.components()[1], "dir2");
}

TEST_F(PathTest, RelativePathConstruction)
{
  Path path("dir1/dir2");
  EXPECT_FALSE(path.isEmpty());
  EXPECT_FALSE(path.isAbsolute());
  ASSERT_EQ(path.components().size(), 2);
  EXPECT_EQ(path.components()[0], "dir1");
  EXPECT_EQ(path.components()[1], "dir2");
}

TEST_F(PathTest, MultipleSlashes)
{
  Path path("///dir1////dir2//");
  EXPECT_TRUE(path.isAbsolute());
  ASSERT_EQ(path.components().size(), 2);
  EXPECT_EQ(path.components()[0], "dir1");
  EXPECT_EQ(path.components()[1], "dir2");
}

TEST_F(PathTest, SingleDot)
{
  Path path("dir1/./dir2");
  auto normalized = path.normalized();
  ASSERT_EQ(normalized.components().size(), 2);
  EXPECT_EQ(normalized.components()[0], "dir1");
  EXPECT_EQ(normalized.components()[1], "dir2");
}

TEST_F(PathTest, DoubleDot)
{
  Path path("dir1/dir2/../dir3");
  auto normalized = path.normalized();
  ASSERT_EQ(normalized.components().size(), 2);
  EXPECT_EQ(normalized.components()[0], "dir1");
  EXPECT_EQ(normalized.components()[1], "dir3");
}

TEST_F(PathTest, MultipleDoubleDots)
{
  Path path("dir1/dir2/../../dir3");
  auto normalized = path.normalized();
  ASSERT_EQ(normalized.components().size(), 1);
  EXPECT_EQ(normalized.components()[0], "dir3");
}

TEST_F(PathTest, DoubleDotAtRoot)
{
  Path path("/dir1/../dir2");
  auto normalized = path.normalized();
  EXPECT_TRUE(normalized.isAbsolute());
  ASSERT_EQ(normalized.components().size(), 1);
  EXPECT_EQ(normalized.components()[0], "dir2");
}

TEST_F(PathTest, ParentPath)
{
  Path path("dir1/dir2/dir3");
  auto parent = path.parent();
  ASSERT_EQ(parent.components().size(), 2);
  EXPECT_EQ(parent.components()[0], "dir1");
  EXPECT_EQ(parent.components()[1], "dir2");
}

TEST_F(PathTest, JoinPaths)
{
  Path path1("dir1/dir2");
  Path path2("dir3/dir4");
  auto joined = path1.join(path2);
  ASSERT_EQ(joined.components().size(), 4);
  EXPECT_EQ(joined.components()[0], "dir1");
  EXPECT_EQ(joined.components()[1], "dir2");
  EXPECT_EQ(joined.components()[2], "dir3");
  EXPECT_EQ(joined.components()[3], "dir4");
}

TEST_F(PathTest, JoinWithAbsolutePath)
{
  Path path1("dir1/dir2");
  Path path2("/dir3/dir4");
  auto joined = path1.join(path2);
  EXPECT_TRUE(joined.isAbsolute());
  ASSERT_EQ(joined.components().size(), 2);
  EXPECT_EQ(joined.components()[0], "dir3");
  EXPECT_EQ(joined.components()[1], "dir4");
}

TEST_F(PathTest, ToString)
{
  Path path1("/dir1/dir2");
  EXPECT_EQ(path1.toString(), "/dir1/dir2");
  
  Path path2("dir1/dir2");
  EXPECT_EQ(path2.toString(), "dir1/dir2");
  
  Path path3("/");
  EXPECT_EQ(path3.toString(), "/");
  
  Path path4("");
  EXPECT_EQ(path4.toString(), ".");
}
