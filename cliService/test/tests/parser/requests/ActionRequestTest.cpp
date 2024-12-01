#include <gtest/gtest.h>
#include "cliService/parser/requests/ActionRequest.hpp"

using namespace cliService;

TEST(ActionRequest, EmptyString)
{
  ActionRequest request("", ActionRequest::Trigger::Enter);
  EXPECT_TRUE(request.getPath().empty());
  EXPECT_TRUE(request.getArgs().empty());
  EXPECT_FALSE(request.isAbsolutePath());
  EXPECT_EQ(request.getTrigger(), ActionRequest::Trigger::Enter);
}

TEST(ActionRequest, RelativePathNoArgs)
{
  ActionRequest request("relative/path", ActionRequest::Trigger::Enter);
  
  std::vector<std::string> expectedPath = {"relative", "path"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_TRUE(request.getArgs().empty());
  EXPECT_FALSE(request.isAbsolutePath());
  EXPECT_EQ(request.getTrigger(), ActionRequest::Trigger::Enter);
}

TEST(ActionRequest, RelativePathWithTrailingSlash)
{
  ActionRequest request("relative/path/", ActionRequest::Trigger::Enter);
  
  std::vector<std::string> expectedPath = {"relative", "path"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_TRUE(request.getArgs().empty());
  EXPECT_FALSE(request.isAbsolutePath());
  EXPECT_EQ(request.getTrigger(), ActionRequest::Trigger::Enter);
}

TEST(ActionRequest, AbsolutePathNoArgs)
{
  ActionRequest request("/absolute/path", ActionRequest::Trigger::Enter);
  
  std::vector<std::string> expectedPath = {"absolute", "path"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_TRUE(request.getArgs().empty());
  EXPECT_TRUE(request.isAbsolutePath());
  EXPECT_EQ(request.getTrigger(), ActionRequest::Trigger::Enter);
}

TEST(ActionRequest, AbsolutePathWithTrailingSlash)
{
  ActionRequest request("/absolute/path/", ActionRequest::Trigger::Enter);
  
  std::vector<std::string> expectedPath = {"absolute", "path"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_TRUE(request.getArgs().empty());
  EXPECT_TRUE(request.isAbsolutePath());
  EXPECT_EQ(request.getTrigger(), ActionRequest::Trigger::Enter);
}

TEST(ActionRequest, RelativePathWithArgs)
{
  ActionRequest request("relative/path arg1 arg2", ActionRequest::Trigger::Enter);
  
  std::vector<std::string> expectedPath = {"relative", "path"};
  std::vector<std::string> expectedArgs = {"arg1", "arg2"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_EQ(request.getArgs(), expectedArgs);
  EXPECT_FALSE(request.isAbsolutePath());
  EXPECT_EQ(request.getTrigger(), ActionRequest::Trigger::Enter);
}

TEST(ActionRequest, AbsolutePathWithArgs)
{
  ActionRequest request("/absolute/path arg1 arg2 arg3", ActionRequest::Trigger::Enter);
  
  std::vector<std::string> expectedPath = {"absolute", "path"};
  std::vector<std::string> expectedArgs = {"arg1", "arg2", "arg3"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_EQ(request.getArgs(), expectedArgs);
  EXPECT_TRUE(request.isAbsolutePath());
  EXPECT_EQ(request.getTrigger(), ActionRequest::Trigger::Enter);
}

TEST(ActionRequest, PathWithMultipleSlashes)
{
  ActionRequest request("/path///with//multiple///slashes", ActionRequest::Trigger::Enter);
  
  std::vector<std::string> expectedPath = {"path", "with", "multiple", "slashes"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_TRUE(request.getArgs().empty());
  EXPECT_TRUE(request.isAbsolutePath());
  EXPECT_EQ(request.getTrigger(), ActionRequest::Trigger::Enter);
}

TEST(ActionRequest, DeathTestTrailingSlashWithArgs)
{
  EXPECT_DEATH({
    ActionRequest request("relative/path/ arg1 arg2", ActionRequest::Trigger::Enter);
  }, "");
  
  EXPECT_DEATH({
    ActionRequest request("/absolute/path/ arg1 arg2", ActionRequest::Trigger::Enter);
  }, "");
}

// New tests for triggers
TEST(ActionRequest, TabTrigger)
{
  ActionRequest request("some/path", ActionRequest::Trigger::Tab);
  EXPECT_EQ(request.getTrigger(), ActionRequest::Trigger::Tab);
}

TEST(ActionRequest, ArrowUpTrigger)
{
  ActionRequest request("some/path", ActionRequest::Trigger::ArrowUp);
  EXPECT_EQ(request.getTrigger(), ActionRequest::Trigger::ArrowUp);
}

TEST(ActionRequest, ArrowDownTrigger)
{
  ActionRequest request("some/path", ActionRequest::Trigger::ArrowDown);
  EXPECT_EQ(request.getTrigger(), ActionRequest::Trigger::ArrowDown);
}

TEST(ActionRequest, TriggerDoesNotAffectParsing)
{
  // Test that parsing works the same regardless of trigger
  std::vector<ActionRequest::Trigger> triggers = {
    ActionRequest::Trigger::Enter,
    ActionRequest::Trigger::Tab,
    ActionRequest::Trigger::ArrowUp,
    ActionRequest::Trigger::ArrowDown
  };

  const std::string testPath = "/test/path arg1 arg2";
  std::vector<std::string> expectedPath = {"test", "path"};
  std::vector<std::string> expectedArgs = {"arg1", "arg2"};

  for (const auto& trigger : triggers)
  {
    ActionRequest request(testPath, trigger);
    EXPECT_EQ(request.getPath(), expectedPath);
    EXPECT_EQ(request.getArgs(), expectedArgs);
    EXPECT_TRUE(request.isAbsolutePath());
    EXPECT_EQ(request.getTrigger(), trigger);
  }
}
