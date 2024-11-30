#include <gtest/gtest.h>
#include "cliService/actionRequest/ActionRequest.hpp"

using namespace cliService;

TEST(ActionRequest, EmptyString) {
  ActionRequest request("");
  EXPECT_TRUE(request.getPath().empty());
  EXPECT_TRUE(request.getArgs().empty());
  EXPECT_FALSE(request.isAbsolutePath());
}

TEST(ActionRequest, RelativePathNoArgs) {
  ActionRequest request("relative/path");
  
  std::vector<std::string> expectedPath = {"relative", "path"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_TRUE(request.getArgs().empty());
  EXPECT_FALSE(request.isAbsolutePath());
}

TEST(ActionRequest, RelativePathWithTrailingSlash) {
  ActionRequest request("relative/path/");
  
  std::vector<std::string> expectedPath = {"relative", "path"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_TRUE(request.getArgs().empty());
  EXPECT_FALSE(request.isAbsolutePath());
}

TEST(ActionRequest, AbsolutePathNoArgs) {
  ActionRequest request("/absolute/path");
  
  std::vector<std::string> expectedPath = {"absolute", "path"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_TRUE(request.getArgs().empty());
  EXPECT_TRUE(request.isAbsolutePath());
}

TEST(ActionRequest, AbsolutePathWithTrailingSlash) {
  ActionRequest request("/absolute/path/");
  
  std::vector<std::string> expectedPath = {"absolute", "path"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_TRUE(request.getArgs().empty());
  EXPECT_TRUE(request.isAbsolutePath());
}

TEST(ActionRequest, RelativePathWithArgs) {
  ActionRequest request("relative/path arg1 arg2");
  
  std::vector<std::string> expectedPath = {"relative", "path"};
  std::vector<std::string> expectedArgs = {"arg1", "arg2"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_EQ(request.getArgs(), expectedArgs);
  EXPECT_FALSE(request.isAbsolutePath());
}

TEST(ActionRequest, AbsolutePathWithArgs) {
  ActionRequest request("/absolute/path arg1 arg2 arg3");
  
  std::vector<std::string> expectedPath = {"absolute", "path"};
  std::vector<std::string> expectedArgs = {"arg1", "arg2", "arg3"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_EQ(request.getArgs(), expectedArgs);
  EXPECT_TRUE(request.isAbsolutePath());
}

TEST(ActionRequest, PathWithMultipleSlashes) {
  ActionRequest request("/path///with//multiple///slashes");
  
  std::vector<std::string> expectedPath = {"path", "with", "multiple", "slashes"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_TRUE(request.getArgs().empty());
  EXPECT_TRUE(request.isAbsolutePath());
}

TEST(ActionRequest, DeathTestTrailingSlashWithArgs) {
  EXPECT_DEATH({
    ActionRequest request("relative/path/ arg1 arg2");
  }, "");
  
  EXPECT_DEATH({
    ActionRequest request("/absolute/path/ arg1 arg2");
  }, "");
}
