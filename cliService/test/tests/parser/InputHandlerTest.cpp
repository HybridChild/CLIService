#include <gtest/gtest.h>
#include "cliService/parser/InputHandler.hpp"
#include "mock/terminal/TerminalMock.hpp"

using namespace cliService;

class InputHandlerTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    _terminal = std::make_unique<TerminalMock>();
    _handler = std::make_unique<InputHandler>(*_terminal, _cliState);
  }

  std::unique_ptr<TerminalMock> _terminal;
  std::unique_ptr<InputHandler> _handler;
  CLIState _cliState{CLIState::LoggedOut};
};

TEST_F(InputHandlerTest, NoInputReturnsNullopt)
{
  auto result = _handler->service();
  EXPECT_FALSE(result.has_value());
}

TEST_F(InputHandlerTest, SimpleLoginRequest)
{
  _terminal->queueInput("user:pass\r");
  
  auto result = _handler->service();
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

TEST_F(InputHandlerTest, ExitLoginRequest)
{
  _terminal->queueInput("exit\r");
  
  auto result = _handler->service();
  ASSERT_TRUE(result.has_value());
  
  auto* loginRequest = dynamic_cast<LoginRequest*>(result->get());
  ASSERT_NE(loginRequest, nullptr);
  EXPECT_TRUE(loginRequest->isExitRequest());
}

TEST_F(InputHandlerTest, LoggedInActionRequest)
{
  _cliState = CLIState::LoggedIn;
  _terminal->queueInput("/test/path arg1\r");
  
  auto result = _handler->service();
  ASSERT_TRUE(result.has_value());
  
  auto* actionRequest = dynamic_cast<ActionRequest*>(result->get());
  ASSERT_NE(actionRequest, nullptr);
  EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::Enter);
  
  std::vector<std::string> expectedPath = {"test", "path"};
  std::vector<std::string> expectedArgs = {"arg1"};
  EXPECT_EQ(actionRequest->getPath(), expectedPath);
  EXPECT_EQ(actionRequest->getArgs(), expectedArgs);
}

TEST_F(InputHandlerTest, TabCompletion)
{
  _cliState = CLIState::LoggedIn;
  // First queue some input so we have something to complete
  _terminal->queueInput("/test/pa");
  _handler->service();  // Process the input
  
  // Now send tab
  _terminal->queueInput("\t");
  
  auto result = _handler->service();
  ASSERT_TRUE(result.has_value());
  
  auto* actionRequest = dynamic_cast<ActionRequest*>(result->get());
  ASSERT_NE(actionRequest, nullptr);
  EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::Tab);
  
  std::vector<std::string> expectedPath = {"test", "pa"};
  EXPECT_EQ(actionRequest->getPath(), expectedPath);
}

TEST_F(InputHandlerTest, ArrowUpHistory)
{
  _cliState = CLIState::LoggedIn;
  // First queue some input to ensure we have a non-empty buffer
  _terminal->queueInput("command");
  _handler->service();
  
  // Now send arrow up
  _terminal->queueInput("\x1B[A");  // ESC [ A for up arrow
  
  auto result = _handler->service();
  ASSERT_TRUE(result.has_value());
  
  auto* actionRequest = dynamic_cast<ActionRequest*>(result->get());
  ASSERT_NE(actionRequest, nullptr);
  EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::ArrowUp);
}

TEST_F(InputHandlerTest, ArrowDownHistory)
{
  _cliState = CLIState::LoggedIn;
  // First queue some input to ensure we have a non-empty buffer
  _terminal->queueInput("command");
  _handler->service();
  
  // Now send arrow down
  _terminal->queueInput("\x1B[B");  // ESC [ B for down arrow
  
  auto result = _handler->service();
  ASSERT_TRUE(result.has_value());
  
  auto* actionRequest = dynamic_cast<ActionRequest*>(result->get());
  ASSERT_NE(actionRequest, nullptr);
  EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::ArrowDown);
}

TEST_F(InputHandlerTest, Backspace)
{
  _cliState = CLIState::LoggedIn;
  _terminal->queueInput("abcd\x7F\r");  // Type "abcd", backspace, enter
  
  auto result = _handler->service();
  ASSERT_TRUE(result.has_value());
  
  auto* actionRequest = dynamic_cast<ActionRequest*>(result->get());
  ASSERT_NE(actionRequest, nullptr);
  
  std::vector<std::string> expectedPath = {"abc"};
  EXPECT_EQ(actionRequest->getPath(), expectedPath);
}

TEST_F(InputHandlerTest, MultipleServiceCalls)
{
  _cliState = CLIState::LoggedIn;
  _terminal->queueInput("abc");
  
  // First service call - no complete command yet
  auto result = _handler->service();
  EXPECT_FALSE(result.has_value());
  
  // Add enter and service again
  _terminal->queueInput("\r");
  result = _handler->service();
  ASSERT_TRUE(result.has_value());
  
  auto* actionRequest = dynamic_cast<ActionRequest*>(result->get());
  ASSERT_NE(actionRequest, nullptr);
  
  std::vector<std::string> expectedPath = {"abc"};
  EXPECT_EQ(actionRequest->getPath(), expectedPath);
}
