#include "cliService/requests/ActionRequest.hpp"
#include <gtest/gtest.h>

using namespace cliService;

class ActionRequestTest : public ::testing::Test
{
protected:
  void SetUp() override {}
};

TEST_F(ActionRequestTest, EmptyInput)
{
  ActionRequest request("");
  EXPECT_TRUE(request.getPath().isEmpty());
  EXPECT_FALSE(request.getPath().isAbsolute());
  EXPECT_TRUE(request.getArgs().empty());
}

TEST_F(ActionRequestTest, SimplePathNoArgs)
{
  ActionRequest request("dir1/dir2");
  
  EXPECT_FALSE(request.getPath().isEmpty());
  EXPECT_FALSE(request.getPath().isAbsolute());
  ASSERT_EQ(request.getPath().components().size(), 2);
  EXPECT_EQ(request.getPath().components()[0], "dir1");
  EXPECT_EQ(request.getPath().components()[1], "dir2");
  EXPECT_TRUE(request.getArgs().empty());
}

TEST_F(ActionRequestTest, AbsolutePathNoArgs)
{
  ActionRequest request("/dir1/dir2");
  
  EXPECT_FALSE(request.getPath().isEmpty());
  EXPECT_TRUE(request.getPath().isAbsolute());
  ASSERT_EQ(request.getPath().components().size(), 2);
  EXPECT_EQ(request.getPath().components()[0], "dir1");
  EXPECT_EQ(request.getPath().components()[1], "dir2");
  EXPECT_TRUE(request.getArgs().empty());
}

TEST_F(ActionRequestTest, PathWithSingleArg)
{
  ActionRequest request("dir1/dir2 arg1");
  
  EXPECT_FALSE(request.getPath().isEmpty());
  ASSERT_EQ(request.getPath().components().size(), 2);
  EXPECT_EQ(request.getPath().components()[0], "dir1");
  EXPECT_EQ(request.getPath().components()[1], "dir2");
  
  ASSERT_EQ(request.getArgs().size(), 1);
  EXPECT_EQ(request.getArgs()[0], "arg1");
}

TEST_F(ActionRequestTest, PathWithMultipleArgs)
{
  ActionRequest request("dir1/dir2 arg1 arg2 arg3");
  
  EXPECT_FALSE(request.getPath().isEmpty());
  ASSERT_EQ(request.getPath().components().size(), 2);
  EXPECT_EQ(request.getPath().components()[0], "dir1");
  EXPECT_EQ(request.getPath().components()[1], "dir2");
  
  ASSERT_EQ(request.getArgs().size(), 3);
  EXPECT_EQ(request.getArgs()[0], "arg1");
  EXPECT_EQ(request.getArgs()[1], "arg2");
  EXPECT_EQ(request.getArgs()[2], "arg3");
}

TEST_F(ActionRequestTest, PathWithExtraSpaces)
{
  ActionRequest request("dir1/dir2   arg1    arg2   arg3");
  
  EXPECT_FALSE(request.getPath().isEmpty());
  ASSERT_EQ(request.getPath().components().size(), 2);
  EXPECT_EQ(request.getPath().components()[0], "dir1");
  EXPECT_EQ(request.getPath().components()[1], "dir2");
  
  ASSERT_EQ(request.getArgs().size(), 3);
  EXPECT_EQ(request.getArgs()[0], "arg1");
  EXPECT_EQ(request.getArgs()[1], "arg2");
  EXPECT_EQ(request.getArgs()[2], "arg3");
}

TEST_F(ActionRequestTest, SimpleCommand)
{
  ActionRequest request("command");
  
  EXPECT_FALSE(request.getPath().isEmpty());
  ASSERT_EQ(request.getPath().components().size(), 1);
  EXPECT_EQ(request.getPath().components()[0], "command");
  EXPECT_TRUE(request.getArgs().empty());
}

TEST_F(ActionRequestTest, CommandWithArgs)
{
  ActionRequest request("command arg1 arg2");
  
  EXPECT_FALSE(request.getPath().isEmpty());
  ASSERT_EQ(request.getPath().components().size(), 1);
  EXPECT_EQ(request.getPath().components()[0], "command");
  
  ASSERT_EQ(request.getArgs().size(), 2);
  EXPECT_EQ(request.getArgs()[0], "arg1");
  EXPECT_EQ(request.getArgs()[1], "arg2");
}

TEST_F(ActionRequestTest, AbsolutePathWithArgs)
{
  ActionRequest request("/dir1/command arg1 arg2");
  
  EXPECT_TRUE(request.getPath().isAbsolute());
  ASSERT_EQ(request.getPath().components().size(), 2);
  EXPECT_EQ(request.getPath().components()[0], "dir1");
  EXPECT_EQ(request.getPath().components()[1], "command");
  
  ASSERT_EQ(request.getArgs().size(), 2);
  EXPECT_EQ(request.getArgs()[0], "arg1");
  EXPECT_EQ(request.getArgs()[1], "arg2");
}

TEST_F(ActionRequestTest, RootPath)
{
  ActionRequest request("/");
  
  EXPECT_TRUE(request.getPath().isAbsolute());
  EXPECT_TRUE(request.getPath().components().empty());
  EXPECT_TRUE(request.getArgs().empty());
}

TEST_F(ActionRequestTest, ParentPath)
{
  ActionRequest request("..");
  
  EXPECT_FALSE(request.getPath().isEmpty());
  ASSERT_EQ(request.getPath().components().size(), 1);
  EXPECT_EQ(request.getPath().components()[0], "..");
  EXPECT_TRUE(request.getArgs().empty());
}

TEST_F(ActionRequestTest, ComplexPathWithArgs)
{
  ActionRequest request("dir1/../../dir2/command arg1 arg2");
  
  EXPECT_FALSE(request.getPath().isEmpty());
  ASSERT_EQ(request.getPath().components().size(), 5);  // Now expecting 5 components
  EXPECT_EQ(request.getPath().components()[0], "dir1");
  EXPECT_EQ(request.getPath().components()[1], "..");
  EXPECT_EQ(request.getPath().components()[2], "..");
  EXPECT_EQ(request.getPath().components()[3], "dir2");
  EXPECT_EQ(request.getPath().components()[4], "command");  // Split properly
  
  ASSERT_EQ(request.getArgs().size(), 2);
  EXPECT_EQ(request.getArgs()[0], "arg1");
  EXPECT_EQ(request.getArgs()[1], "arg2");
}
