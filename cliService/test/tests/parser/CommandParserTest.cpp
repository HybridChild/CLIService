#include <gtest/gtest.h>
#include "cliService/parser/CommandParser.hpp"
#include "mock/terminal/TerminalMock.hpp"

using namespace cliService;

class CommandParserTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    _terminal = std::make_unique<TerminalMock>();
    _parser = std::make_unique<CommandParser>(*_terminal, _cliState);
  }

  std::unique_ptr<TerminalMock> _terminal;
  std::unique_ptr<CommandParser> _parser;
  CLIState _cliState{CLIState::LoggedOut};
};

TEST_F(CommandParserTest, NoInputReturnsNullopt)
{
  auto result = _parser->service();
  EXPECT_FALSE(result.has_value());
}

TEST_F(CommandParserTest, SimpleLoginRequest)
{
  _terminal->queueInput("user:pass\n");
  
  auto result = _parser->service();
  ASSERT_TRUE(result.has_value());
  
  auto* loginRequest = dynamic_cast<LoginRequest*>(result->get());
  ASSERT_NE(loginRequest, nullptr);
  EXPECT_EQ(loginRequest->getUsername(), "user");
  EXPECT_EQ(loginRequest->getPassword(), "pass");
  
  std::string output = _terminal->getOutput();
  EXPECT_TRUE(output.find("user:") != std::string::npos);
  EXPECT_TRUE(output.find("pass") == std::string::npos);
  EXPECT_TRUE(output.find("****") != std::string::npos);
}

TEST_F(CommandParserTest, LoggedInActionRequest)
{
  _cliState = CLIState::LoggedIn;
  _terminal->queueInput("/test/path arg1\n");
  
  auto result = _parser->service();
  ASSERT_TRUE(result.has_value());
  
  auto* actionRequest = dynamic_cast<ActionRequest*>(result->get());
  ASSERT_NE(actionRequest, nullptr);
  
  std::vector<std::string> expectedPath = {"test", "path"};
  std::vector<std::string> expectedArgs = {"arg1"};
  EXPECT_EQ(actionRequest->getPath(), expectedPath);
  EXPECT_EQ(actionRequest->getArgs(), expectedArgs);
}

TEST_F(CommandParserTest, TabKey)
{
  _cliState = CLIState::LoggedIn;
  _terminal->queueInput("\t");
  
  auto result = _parser->service();
  ASSERT_TRUE(result.has_value());
  
  auto* actionRequest = dynamic_cast<ActionRequest*>(result->get());
  ASSERT_NE(actionRequest, nullptr);
  
  std::vector<std::string> expectedPath = {"key:tab"};
  EXPECT_EQ(actionRequest->getPath(), expectedPath);
  EXPECT_TRUE(actionRequest->getArgs().empty());
}

TEST_F(CommandParserTest, ArrowKeys)
{
  _cliState = CLIState::LoggedIn;
  
  // Test all arrow keys
  std::vector<std::pair<std::string, std::string>> arrowTests = {
    {"\x1B[A", "key:up"},    // Up arrow
    {"\x1B[B", "key:down"},  // Down arrow
    {"\x1B[C", "key:right"}, // Right arrow
    {"\x1B[D", "key:left"}   // Left arrow
  };
  
  for (const auto& [input, expectedKey] : arrowTests) {
    _terminal->queueInput(input);
    
    auto result = _parser->service();
    ASSERT_TRUE(result.has_value());
    
    auto* actionRequest = dynamic_cast<ActionRequest*>(result->get());
    ASSERT_NE(actionRequest, nullptr);
    
    std::vector<std::string> expectedPath = {expectedKey};
    EXPECT_EQ(actionRequest->getPath(), expectedPath);
    EXPECT_TRUE(actionRequest->getArgs().empty());
  }
}

TEST_F(CommandParserTest, Backspace)
{
  _cliState = CLIState::LoggedIn;
  _terminal->queueInput("abcd\x7F\n");  // Type "abcd", backspace, enter
  
  auto result = _parser->service();
  ASSERT_TRUE(result.has_value());
  
  auto* actionRequest = dynamic_cast<ActionRequest*>(result->get());
  ASSERT_NE(actionRequest, nullptr);
  
  std::vector<std::string> expectedPath = {"abc"};
  EXPECT_EQ(actionRequest->getPath(), expectedPath);
}

TEST_F(CommandParserTest, MultipleServiceCalls)
{
  _cliState = CLIState::LoggedIn;
  _terminal->queueInput("abc");
  
  // First service call - no complete command yet
  auto result = _parser->service();
  EXPECT_FALSE(result.has_value());
  
  // Add enter and service again
  _terminal->queueInput("\n");
  result = _parser->service();
  ASSERT_TRUE(result.has_value());
  
  auto* actionRequest = dynamic_cast<ActionRequest*>(result->get());
  ASSERT_NE(actionRequest, nullptr);
  
  std::vector<std::string> expectedPath = {"abc"};
  EXPECT_EQ(actionRequest->getPath(), expectedPath);
}

TEST_F(CommandParserTest, SpecialKeysOnlyInLoggedInState)
{
  _cliState = CLIState::LoggedOut;
  
  // Test tab key
  _terminal->queueInput("\t");
  auto result = _parser->service();
  EXPECT_FALSE(result.has_value());
  
  // Test arrow keys
  _terminal->queueInput("\x1B[A"); // Up arrow
  result = _parser->service();
  EXPECT_FALSE(result.has_value());
}
