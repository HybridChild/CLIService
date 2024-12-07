#include "cliService/cli/InputParser.hpp"
#include "cliService/cli/ActionRequest.hpp"
#include "cliService/cli/LoginRequest.hpp"
#include "mock/terminal/TerminalMock.hpp"
#include <gtest/gtest.h>

using namespace cliService;

class InputParserTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    _currentState = CLIState::LoggedIn;
    _parser = std::make_unique<InputParser>(_terminal, _currentState);
  }

  // Helper to process all queued input
  std::optional<std::unique_ptr<RequestBase>> processAllInput()
  {
    std::optional<std::unique_ptr<RequestBase>> request;
    while (_terminal.available())
    {
      request = _parser->service();
    }
    return request;
  }

  TerminalMock _terminal;
  CLIState _currentState;
  std::unique_ptr<InputParser> _parser;
};

TEST_F(InputParserTest, EmptyInput)
{
  _terminal.queueInput("\n");
  auto request = processAllInput();
  EXPECT_FALSE(request.has_value());
  EXPECT_EQ(_terminal.getOutput(), "\n");
}

TEST_F(InputParserTest, SimpleCommand)
{
  _terminal.queueInput("command\n");
  auto request = processAllInput();
  
  ASSERT_TRUE(request.has_value());
  auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
  ASSERT_NE(actionRequest, nullptr);
  EXPECT_EQ(actionRequest->getPath().elements()[0], "command");
  EXPECT_EQ(_terminal.getOutput(), "command\n");
}

TEST_F(InputParserTest, CommandWithArguments)
{
  _terminal.queueInput("command arg1 arg2\n");
  auto request = processAllInput();
  
  ASSERT_TRUE(request.has_value());
  auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
  ASSERT_NE(actionRequest, nullptr);
  EXPECT_EQ(actionRequest->getPath().elements()[0], "command");
  ASSERT_EQ(actionRequest->getArgs().size(), 2);
  EXPECT_EQ(actionRequest->getArgs()[0], "arg1");
  EXPECT_EQ(actionRequest->getArgs()[1], "arg2");
}

TEST_F(InputParserTest, BackspaceHandling)
{
  _terminal.queueInput("commanf");
  processAllInput();
  EXPECT_EQ(_terminal.getOutput(), "commanf");
  
  _terminal.clearOutput();
  _terminal.queueInput(std::string(1, InputParser::BACKSPACE));
  processAllInput();
  EXPECT_EQ(_terminal.getOutput(), "\b \b");
  
  _terminal.clearOutput();
  _terminal.queueInput("d\n");
  auto request = processAllInput();
  EXPECT_EQ(_terminal.getOutput(), "d\n");
  
  ASSERT_TRUE(request.has_value());
  auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
  ASSERT_NE(actionRequest, nullptr);
  EXPECT_EQ(actionRequest->getPath().elements()[0], "command");
}

TEST_F(InputParserTest, LoginHandling)
{
  _currentState = CLIState::LoggedOut;
  _parser = std::make_unique<InputParser>(_terminal, _currentState);
  
  _terminal.queueInput("user:pass\n");
  auto request = processAllInput();
  
  ASSERT_TRUE(request.has_value());
  auto* loginRequest = dynamic_cast<LoginRequest*>(request.value().get());
  ASSERT_NE(loginRequest, nullptr);
  EXPECT_EQ(loginRequest->getUsername(), "user");
  EXPECT_EQ(loginRequest->getPassword(), "pass");
  
  // Verify password masking
  std::string expectedOutput = "user:****\n";
  EXPECT_EQ(_terminal.getOutput(), expectedOutput);
}

TEST_F(InputParserTest, ArrowKeys)
{
  // Test UP arrow
  _terminal.queueInput({0x1B, '[', 'A'});  // ESC [ A
  auto request = processAllInput();
  
  ASSERT_TRUE(request.has_value());
  auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
  ASSERT_NE(actionRequest, nullptr);
  EXPECT_EQ(actionRequest->getPath().elements()[0], InputParser::KEY_CODE_ARROW_UP);
  
  // Test DOWN arrow
  _terminal.clearOutput();
  _terminal.queueInput({0x1B, '[', 'B'});  // ESC [ B
  request = processAllInput();
  
  ASSERT_TRUE(request.has_value());
  actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
  ASSERT_NE(actionRequest, nullptr);
  EXPECT_EQ(actionRequest->getPath().elements()[0], InputParser::KEY_CODE_ARROW_DOWN);

  // Test LEFT arrow
  _terminal.clearOutput();
  _terminal.queueInput({0x1B, '[', 'D'});  // ESC [ D
  request = processAllInput();

  ASSERT_TRUE(request.has_value());
  actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
  ASSERT_NE(actionRequest, nullptr);
  EXPECT_EQ(actionRequest->getPath().elements()[0], InputParser::KEY_CODE_ARROW_LEFT);

  // Test RIGHT arrow
  _terminal.clearOutput();
  _terminal.queueInput({0x1B, '[', 'C'});  // ESC [ C
  request = processAllInput();

  ASSERT_TRUE(request.has_value());
  actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
  ASSERT_NE(actionRequest, nullptr);
  EXPECT_EQ(actionRequest->getPath().elements()[0], InputParser::KEY_CODE_ARROW_RIGHT);
}

TEST_F(InputParserTest, TabKey)
{
  _terminal.queueInput(std::string(1, InputParser::TAB));
  auto request = processAllInput();
  
  ASSERT_TRUE(request.has_value());
  auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
  ASSERT_NE(actionRequest, nullptr);
  EXPECT_EQ(actionRequest->getPath().elements()[0], InputParser::KEY_CODE_TAB);
}

TEST_F(InputParserTest, MultipleBackspace)
{
  _terminal.queueInput("hello");
  processAllInput();
  EXPECT_EQ(_terminal.getOutput(), "hello");
  
  _terminal.clearOutput();
  _terminal.queueInput(std::string(1, InputParser::BACKSPACE));
  processAllInput();
  EXPECT_EQ(_terminal.getOutput(), "\b \b");
  
  _terminal.clearOutput();
  _terminal.queueInput(std::string(1, InputParser::BACKSPACE));
  processAllInput();
  EXPECT_EQ(_terminal.getOutput(), "\b \b");
  
  _terminal.clearOutput();
  _terminal.queueInput("\n");
  auto request = processAllInput();
  
  ASSERT_TRUE(request.has_value());
  auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
  ASSERT_NE(actionRequest, nullptr);
  EXPECT_EQ(actionRequest->getPath().elements()[0], "hel");
}
