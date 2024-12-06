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
  
  // Don't check exact output string as it might vary by platform
  std::string output = _terminal->getOutput();
  EXPECT_TRUE(output.find("user:") != std::string::npos);
  EXPECT_TRUE(output.find("pass") == std::string::npos);  // Password should be masked
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
  EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::Enter);
  
  std::vector<std::string> expectedPath = {"test", "path"};
  std::vector<std::string> expectedArgs = {"arg1"};
  EXPECT_EQ(actionRequest->getPath(), expectedPath);
  EXPECT_EQ(actionRequest->getArgs(), expectedArgs);
}

TEST_F(CommandParserTest, TabCompletion)
{
  _cliState = CLIState::LoggedIn;
  // First queue some input so we have something to complete
  _terminal->queueInput("/test/pa");
  _parser->service();  // Process the input
  
  // Now send tab
  _terminal->queueInput("\t");
  
  auto result = _parser->service();
  ASSERT_TRUE(result.has_value());
  
  auto* actionRequest = dynamic_cast<ActionRequest*>(result->get());
  ASSERT_NE(actionRequest, nullptr);
  EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::Tab);
  
  std::vector<std::string> expectedPath = {"test", "pa"};
  EXPECT_EQ(actionRequest->getPath(), expectedPath);
}

TEST_F(CommandParserTest, ArrowUpHistory)
{
  _cliState = CLIState::LoggedIn;
  // First queue some input to ensure we have a non-empty buffer
  _terminal->queueInput("command");
  _parser->service();
  
  // Now send arrow up
  _terminal->queueInput("\x1B[A");  // ESC [ A for up arrow
  
  auto result = _parser->service();
  ASSERT_TRUE(result.has_value());
  
  auto* actionRequest = dynamic_cast<ActionRequest*>(result->get());
  ASSERT_NE(actionRequest, nullptr);
  EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::ArrowUp);
}

TEST_F(CommandParserTest, ArrowDownHistory)
{
  _cliState = CLIState::LoggedIn;
  // First queue some input to ensure we have a non-empty buffer
  _terminal->queueInput("command");
  _parser->service();
  
  // Now send arrow down
  _terminal->queueInput("\x1B[B");  // ESC [ B for down arrow
  
  auto result = _parser->service();
  ASSERT_TRUE(result.has_value());
  
  auto* actionRequest = dynamic_cast<ActionRequest*>(result->get());
  ASSERT_NE(actionRequest, nullptr);
  EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::ArrowDown);
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
