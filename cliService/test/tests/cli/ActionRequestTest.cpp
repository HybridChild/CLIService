#include "cliService/cli/ActionRequest.hpp"
#include <gtest/gtest.h>

namespace cliService
{

  class ActionRequestTest : public ::testing::Test
  {
  protected:
    void SetUp() override {}
  };

  TEST_F(ActionRequestTest, EmptyInput)
  {
    ActionRequest request("", ActionRequest::Trigger::Enter);
    EXPECT_TRUE(request.getPath().isEmpty());
    EXPECT_FALSE(request.getPath().isAbsolute());
    EXPECT_TRUE(request.getArgs().empty());
  }

  TEST_F(ActionRequestTest, RelativePathNoArgs)
  {
    ActionRequest request("dir1/dir2", ActionRequest::Trigger::Enter);
    
    EXPECT_FALSE(request.getPath().isEmpty());
    EXPECT_FALSE(request.getPath().isAbsolute());
    ASSERT_EQ(request.getPath().elements().size(), 2);
    EXPECT_EQ(request.getPath().elements()[0], "dir1");
    EXPECT_EQ(request.getPath().elements()[1], "dir2");
    EXPECT_TRUE(request.getArgs().empty());
  }

  TEST_F(ActionRequestTest, AbsolutePathNoArgs)
  {
    ActionRequest request("/dir1/dir2", ActionRequest::Trigger::Enter);
    
    EXPECT_FALSE(request.getPath().isEmpty());
    EXPECT_TRUE(request.getPath().isAbsolute());
    ASSERT_EQ(request.getPath().elements().size(), 2);
    EXPECT_EQ(request.getPath().elements()[0], "dir1");
    EXPECT_EQ(request.getPath().elements()[1], "dir2");
    EXPECT_TRUE(request.getArgs().empty());
  }

  TEST_F(ActionRequestTest, RelativePathWithSingleArg)
  {
    ActionRequest request("dir1/dir2 arg1", ActionRequest::Trigger::Enter);
    
    EXPECT_FALSE(request.getPath().isEmpty());
    EXPECT_FALSE(request.getPath().isAbsolute());
    ASSERT_EQ(request.getPath().elements().size(), 2);
    EXPECT_EQ(request.getPath().elements()[0], "dir1");
    EXPECT_EQ(request.getPath().elements()[1], "dir2");
    ASSERT_EQ(request.getArgs().size(), 1);
    EXPECT_EQ(request.getArgs()[0], "arg1");
  }

  TEST_F(ActionRequestTest, RelativePathWithMultipleArgs)
  {
    ActionRequest request("dir1/dir2 arg1 arg2 arg3", ActionRequest::Trigger::Enter);
    
    EXPECT_FALSE(request.getPath().isEmpty());
    EXPECT_FALSE(request.getPath().isAbsolute());
    ASSERT_EQ(request.getPath().elements().size(), 2);
    EXPECT_EQ(request.getPath().elements()[0], "dir1");
    EXPECT_EQ(request.getPath().elements()[1], "dir2");
    
    ASSERT_EQ(request.getArgs().size(), 3);
    EXPECT_EQ(request.getArgs()[0], "arg1");
    EXPECT_EQ(request.getArgs()[1], "arg2");
    EXPECT_EQ(request.getArgs()[2], "arg3");
  }

  TEST_F(ActionRequestTest, AbsolutePathWithArgs)
  {
    ActionRequest request("/dir1/command arg1 arg2", ActionRequest::Trigger::Enter);
    
    EXPECT_TRUE(request.getPath().isAbsolute());
    ASSERT_EQ(request.getPath().elements().size(), 2);
    EXPECT_EQ(request.getPath().elements()[0], "dir1");
    EXPECT_EQ(request.getPath().elements()[1], "command");
    
    ASSERT_EQ(request.getArgs().size(), 2);
    EXPECT_EQ(request.getArgs()[0], "arg1");
    EXPECT_EQ(request.getArgs()[1], "arg2");
  }

  TEST_F(ActionRequestTest, PathWithExtraSpacesBetweenArgs)
  {
    ActionRequest request("dir1/dir2   arg1    arg2   arg3", ActionRequest::Trigger::Enter);
    
    EXPECT_FALSE(request.getPath().isEmpty());
    ASSERT_EQ(request.getPath().elements().size(), 2);
    EXPECT_EQ(request.getPath().elements()[0], "dir1");
    EXPECT_EQ(request.getPath().elements()[1], "dir2");
    
    ASSERT_EQ(request.getArgs().size(), 3);
    EXPECT_EQ(request.getArgs()[0], "arg1");
    EXPECT_EQ(request.getArgs()[1], "arg2");
    EXPECT_EQ(request.getArgs()[2], "arg3");
  }

  TEST_F(ActionRequestTest, SimpleCommand)
  {
    ActionRequest request("command", ActionRequest::Trigger::Enter);
    
    EXPECT_FALSE(request.getPath().isEmpty());
    ASSERT_EQ(request.getPath().elements().size(), 1);
    EXPECT_EQ(request.getPath().elements()[0], "command");
    EXPECT_TRUE(request.getArgs().empty());
  }

  TEST_F(ActionRequestTest, CommandWithArgs)
  {
    ActionRequest request("command arg1 arg2", ActionRequest::Trigger::Enter);
    
    EXPECT_FALSE(request.getPath().isEmpty());
    ASSERT_EQ(request.getPath().elements().size(), 1);
    EXPECT_EQ(request.getPath().elements()[0], "command");
    
    ASSERT_EQ(request.getArgs().size(), 2);
    EXPECT_EQ(request.getArgs()[0], "arg1");
    EXPECT_EQ(request.getArgs()[1], "arg2");
  }

  TEST_F(ActionRequestTest, RootPath)
  {
    ActionRequest request("/", ActionRequest::Trigger::Enter);
    
    EXPECT_TRUE(request.getPath().isAbsolute());
    EXPECT_TRUE(request.getPath().elements().empty());
    EXPECT_TRUE(request.getArgs().empty());
  }

  TEST_F(ActionRequestTest, ParentPath)
  {
    ActionRequest request("..", ActionRequest::Trigger::Enter);
    
    EXPECT_FALSE(request.getPath().isEmpty());
    ASSERT_EQ(request.getPath().elements().size(), 1);
    EXPECT_EQ(request.getPath().elements()[0], "..");
    EXPECT_TRUE(request.getArgs().empty());
  }

  TEST_F(ActionRequestTest, ComplexPathWithArgs)
  {
    ActionRequest request("dir1/../../dir2/command arg1 arg2", ActionRequest::Trigger::Enter);
    
    EXPECT_FALSE(request.getPath().isEmpty());
    ASSERT_EQ(request.getPath().elements().size(), 5);  // Now expecting 5 elements
    EXPECT_EQ(request.getPath().elements()[0], "dir1");
    EXPECT_EQ(request.getPath().elements()[1], "..");
    EXPECT_EQ(request.getPath().elements()[2], "..");
    EXPECT_EQ(request.getPath().elements()[3], "dir2");
    EXPECT_EQ(request.getPath().elements()[4], "command");  // Split properly
    
    ASSERT_EQ(request.getArgs().size(), 2);
    EXPECT_EQ(request.getArgs()[0], "arg1");
    EXPECT_EQ(request.getArgs()[1], "arg2");
  }

}
