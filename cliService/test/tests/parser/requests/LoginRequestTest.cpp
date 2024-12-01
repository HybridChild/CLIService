#include "cliService/parser/requests/LoginRequest.hpp"
#include <gtest/gtest.h>

using namespace cliService;

class LoginRequestTest : public ::testing::Test
{
protected:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(LoginRequestTest, ValidLoginRequest)
{
  LoginRequest request("user123:pass456");
  
  EXPECT_FALSE(request.isExitRequest());
  EXPECT_EQ(request.getUsername(), "user123");
  EXPECT_EQ(request.getPassword(), "pass456");
}

TEST_F(LoginRequestTest, ExitRequest)
{
  LoginRequest request("exit");
  
  EXPECT_TRUE(request.isExitRequest());
  EXPECT_TRUE(request.getUsername().empty());
  EXPECT_TRUE(request.getPassword().empty());
}

TEST_F(LoginRequestTest, EmptyUsername)
{
  EXPECT_DEATH({
    LoginRequest request(":password");
  }, "Username cannot be empty");
}

TEST_F(LoginRequestTest, EmptyPassword)
{
  EXPECT_DEATH({
    LoginRequest request("username:");
  }, "Password cannot be empty");
}

TEST_F(LoginRequestTest, NoDelimiter)
{
  EXPECT_DEATH({
    LoginRequest request("invalidformat");
  }, "Invalid login format");
}

TEST_F(LoginRequestTest, MultipleDelimiters)
{
  LoginRequest request("user:pass:extra");
  
  EXPECT_FALSE(request.isExitRequest());
  EXPECT_EQ(request.getUsername(), "user");
  EXPECT_EQ(request.getPassword(), "pass:extra");
}

TEST_F(LoginRequestTest, WhitespaceHandling)
{
  LoginRequest request(" user : pass ");
  
  EXPECT_FALSE(request.isExitRequest());
  EXPECT_EQ(request.getUsername(), " user ");
  EXPECT_EQ(request.getPassword(), " pass ");
}

TEST_F(LoginRequestTest, SpecialCharacters)
{
  LoginRequest request("user@domain.com:p@ssw0rd!");
  
  EXPECT_FALSE(request.isExitRequest());
  EXPECT_EQ(request.getUsername(), "user@domain.com");
  EXPECT_EQ(request.getPassword(), "p@ssw0rd!");
}

TEST_F(LoginRequestTest, NonExactExitStrings)
{
  // Test variations of "exit" that should be treated as regular login attempts
  const std::string testCases[] = {
    "EXIT",
    "Exit",
    "exit ",
    " exit",
    "exitnow"
  };

  for (const auto& input : testCases)
  {
    EXPECT_DEATH({
      LoginRequest request(input);
      request.getUsername(); // This should trigger the delimiter assertion
    }, "Invalid login format") << "Test case: " << input;
  }
}
