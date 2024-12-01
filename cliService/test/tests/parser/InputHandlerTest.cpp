#include <gtest/gtest.h>
#include "cliService/parser/InputHandler.hpp"
#include "mock/terminal/TerminalMock.hpp"

using namespace cliService;

class InputHandlerTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    _io = std::make_unique<TerminalMock>();
    _handler = std::make_unique<InputHandler>(*_io, _cliState);
  }

  std::unique_ptr<TerminalMock> _io;
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
  _io->queueInput("user:pass\r");
  
  auto result = _handler->service();
  ASSERT_TRUE(result.has_value());
  
  auto* loginRequest = dynamic_cast<LoginRequest*>(result->get());
  ASSERT_NE(loginRequest, nullptr);
  EXPECT_EQ(loginRequest->getUsername(), "user");
  EXPECT_EQ(loginRequest->getPassword(), "pass");
  
  // Don't check exact output string as it might vary by platform
  std::string output = _io->getOutput();
  EXPECT_TRUE(output.find("user:") != std::string::npos);
  EXPECT_TRUE(output.find("pass") == std::string::npos);  // Password should be masked
  EXPECT_TRUE(output.find("****") != std::string::npos);
}

TEST_F(InputHandlerTest, ExitLoginRequest)
{
  _io->queueInput("exit\r");
  
  auto result = _handler->service();
  ASSERT_TRUE(result.has_value());
  
  auto* loginRequest = dynamic_cast<LoginRequest*>(result->get());
  ASSERT_NE(loginRequest, nullptr);
  EXPECT_TRUE(loginRequest->isExitRequest());
}

TEST_F(InputHandlerTest, LoggedInActionRequest)
{
  _cliState = CLIState::LoggedIn;
  _io->queueInput("/test/path arg1\r");
  
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
  _io->queueInput("/test/pa");
  _handler->service();  // Process the input
  
  // Now send tab
  _io->queueInput("\t");
  
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
  _io->queueInput("command");
  _handler->service();
  
  // Now send arrow up
  _io->queueInput("\x1B[A");  // ESC [ A for up arrow
  
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
  _io->queueInput("command");
  _handler->service();
  
  // Now send arrow down
  _io->queueInput("\x1B[B");  // ESC [ B for down arrow
  
  auto result = _handler->service();
  ASSERT_TRUE(result.has_value());
  
  auto* actionRequest = dynamic_cast<ActionRequest*>(result->get());
  ASSERT_NE(actionRequest, nullptr);
  EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::ArrowDown);
}

TEST_F(InputHandlerTest, Backspace)
{
  _cliState = CLIState::LoggedIn;
  _io->queueInput("abcd\x7F\r");  // Type "abcd", backspace, enter
  
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
  _io->queueInput("abc");
  
  // First service call - no complete command yet
  auto result = _handler->service();
  EXPECT_FALSE(result.has_value());
  
  // Add enter and service again
  _io->queueInput("\r");
  result = _handler->service();
  ASSERT_TRUE(result.has_value());
  
  auto* actionRequest = dynamic_cast<ActionRequest*>(result->get());
  ASSERT_NE(actionRequest, nullptr);
  
  std::vector<std::string> expectedPath = {"abc"};
  EXPECT_EQ(actionRequest->getPath(), expectedPath);
}
