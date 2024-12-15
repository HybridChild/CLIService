#include "cliService/cli/LoginRequest.hpp"
#include <gtest/gtest.h>

namespace cliService
{

  class LoginRequestTest : public ::testing::Test
  {
  protected:
    void SetUp() override {}
    void TearDown() override {}
  };

  TEST_F(LoginRequestTest, ValidLoginRequest)
  {
    auto request = LoginRequest::create("user123:pass456");
    ASSERT_TRUE(request.has_value());
    EXPECT_EQ(request->getUsername(), "user123");
    EXPECT_EQ(request->getPassword(), "pass456");
  }

  TEST_F(LoginRequestTest, EmptyUsername)
  {
    auto request = LoginRequest::create(":password");
    EXPECT_FALSE(request.has_value());
  }

  TEST_F(LoginRequestTest, EmptyPassword)
  {
    auto request = LoginRequest::create("username:");
    EXPECT_FALSE(request.has_value());
  }

  TEST_F(LoginRequestTest, NoDelimiter)
  {
    auto request = LoginRequest::create("invalidformat");
    EXPECT_FALSE(request.has_value());
  }

  TEST_F(LoginRequestTest, MultipleDelimiters)
  {
    auto request = LoginRequest::create("user:pass:extra");
    ASSERT_TRUE(request.has_value());
    EXPECT_EQ(request->getUsername(), "user");
    EXPECT_EQ(request->getPassword(), "pass:extra");
  }

  TEST_F(LoginRequestTest, WhitespaceHandling)
  {
    auto request = LoginRequest::create(" user : pass ");
    ASSERT_TRUE(request.has_value());
    EXPECT_EQ(request->getUsername(), " user ");
    EXPECT_EQ(request->getPassword(), " pass ");
  }

  TEST_F(LoginRequestTest, SpecialCharacters)
  {
    auto request = LoginRequest::create("user@domain.com:p@ssw0rd!");
    ASSERT_TRUE(request.has_value());
    EXPECT_EQ(request->getUsername(), "user@domain.com");
    EXPECT_EQ(request->getPassword(), "p@ssw0rd!");
  }

}
