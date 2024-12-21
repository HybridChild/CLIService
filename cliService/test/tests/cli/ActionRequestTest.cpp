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

  TEST_F(ActionRequestTest, PathWithTrailingSlashAndArgs)
  {
    // Test single trailing slash
    ActionRequest request1("dir1/command/ arg1 arg2", ActionRequest::Trigger::Enter);

    EXPECT_FALSE(request1.getPath().isEmpty());
    ASSERT_EQ(request1.getPath().elements().size(), 2);
    EXPECT_EQ(request1.getPath().elements()[0], "dir1");
    EXPECT_EQ(request1.getPath().elements()[1], "command");
    ASSERT_EQ(request1.getArgs().size(), 2);
    EXPECT_EQ(request1.getArgs()[0], "arg1");
    EXPECT_EQ(request1.getArgs()[1], "arg2");

    // Test multiple trailing slashes
    ActionRequest request2("dir1/command/// arg1 arg2", ActionRequest::Trigger::Enter);

    EXPECT_FALSE(request2.getPath().isEmpty());
    ASSERT_EQ(request2.getPath().elements().size(), 2);
    EXPECT_EQ(request2.getPath().elements()[0], "dir1");
    EXPECT_EQ(request2.getPath().elements()[1], "command");
    ASSERT_EQ(request2.getArgs().size(), 2);
    EXPECT_EQ(request2.getArgs()[0], "arg1");
    EXPECT_EQ(request2.getArgs()[1], "arg2");

    // Test absolute path with trailing slash
    ActionRequest request3("/dir1/command/ arg1 arg2", ActionRequest::Trigger::Enter);

    EXPECT_TRUE(request3.getPath().isAbsolute());
    ASSERT_EQ(request3.getPath().elements().size(), 2);
    EXPECT_EQ(request3.getPath().elements()[0], "dir1");
    EXPECT_EQ(request3.getPath().elements()[1], "command");
    ASSERT_EQ(request3.getArgs().size(), 2);
    EXPECT_EQ(request3.getArgs()[0], "arg1");
    EXPECT_EQ(request3.getArgs()[1], "arg2");

    // Verify trailing slash is preserved when no args
    ActionRequest request4("dir1/command/", ActionRequest::Trigger::Enter);

    EXPECT_FALSE(request4.getPath().isEmpty());
    ASSERT_EQ(request4.getPath().elements().size(), 2);
    EXPECT_EQ(request4.getPath().elements()[0], "dir1");
    EXPECT_EQ(request4.getPath().elements()[1], "command");
    EXPECT_TRUE(request4.getArgs().empty());
  }

}
