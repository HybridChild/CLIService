#include <gtest/gtest.h>
#include "CliRequest.hpp"

using namespace cliService;

TEST(CliRequest, EmptyString) {
  CliRequest request("");
  EXPECT_TRUE(request.getPath().empty());
  EXPECT_TRUE(request.getArgs().empty());
  EXPECT_FALSE(request.isAbsolutePath());
}

TEST(CliRequest, RelativePathNoArgs) {
  CliRequest request("relative/path");
  
  std::vector<std::string> expectedPath = {"relative", "path"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_TRUE(request.getArgs().empty());
  EXPECT_FALSE(request.isAbsolutePath());
}

TEST(CliRequest, RelativePathWithTrailingSlash) {
  CliRequest request("relative/path/");
  
  std::vector<std::string> expectedPath = {"relative", "path"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_TRUE(request.getArgs().empty());
  EXPECT_FALSE(request.isAbsolutePath());
}

TEST(CliRequest, AbsolutePathNoArgs) {
  CliRequest request("/absolute/path");
  
  std::vector<std::string> expectedPath = {"absolute", "path"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_TRUE(request.getArgs().empty());
  EXPECT_TRUE(request.isAbsolutePath());
}

TEST(CliRequest, AbsolutePathWithTrailingSlash) {
  CliRequest request("/absolute/path/");
  
  std::vector<std::string> expectedPath = {"absolute", "path"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_TRUE(request.getArgs().empty());
  EXPECT_TRUE(request.isAbsolutePath());
}

TEST(CliRequest, RelativePathWithArgs) {
  CliRequest request("relative/path arg1 arg2");
  
  std::vector<std::string> expectedPath = {"relative", "path"};
  std::vector<std::string> expectedArgs = {"arg1", "arg2"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_EQ(request.getArgs(), expectedArgs);
  EXPECT_FALSE(request.isAbsolutePath());
}

TEST(CliRequest, AbsolutePathWithArgs) {
  CliRequest request("/absolute/path arg1 arg2 arg3");
  
  std::vector<std::string> expectedPath = {"absolute", "path"};
  std::vector<std::string> expectedArgs = {"arg1", "arg2", "arg3"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_EQ(request.getArgs(), expectedArgs);
  EXPECT_TRUE(request.isAbsolutePath());
}

TEST(CliRequest, PathWithMultipleSlashes) {
  CliRequest request("/path///with//multiple///slashes");
  
  std::vector<std::string> expectedPath = {"path", "with", "multiple", "slashes"};
  EXPECT_EQ(request.getPath(), expectedPath);
  EXPECT_TRUE(request.getArgs().empty());
  EXPECT_TRUE(request.isAbsolutePath());
}

TEST(CliRequest, DeathTestTrailingSlashWithArgs) {
  EXPECT_DEATH({
    CliRequest request("relative/path/ arg1 arg2");
  }, "");
  
  EXPECT_DEATH({
    CliRequest request("/absolute/path/ arg1 arg2");
  }, "");
}
