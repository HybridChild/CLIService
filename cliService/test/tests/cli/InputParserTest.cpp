#include "cliService/cli/InputParser.hpp"
#include "cliService/cli/ActionRequest.hpp"
#include "cliService/cli/LoginRequest.hpp"
#include "mock/terminal/TerminalMock.hpp"
#include <gtest/gtest.h>

namespace cliService
{

  class InputParserTest : public ::testing::Test
  {
  protected:
    void SetUp() override
    {
      _currentState = CLIState::LoggedIn;
      _parser = std::make_unique<InputParser>(_terminal, _currentState);
    }

    TerminalMock _terminal;
    CLIState _currentState;
    std::unique_ptr<InputParser> _parser;

    // Helper to process all queued input
    std::optional<std::unique_ptr<RequestBase>> processAllInput()
    {
      std::optional<std::unique_ptr<RequestBase>> request;

      while (_terminal.available()) {
        request = _parser->parseNextRequest();
      }

      return request;
    }

    // Helper to generate the terminal sequence for clearing a string
    std::string generateClearSequence(size_t length) {
      std::string sequence;
      for (size_t i = 0; i < length; ++i) {
        sequence += "\b \b";  // backspace, space, backspace for each character
      }
      return sequence;
    }
  };

  TEST_F(InputParserTest, EmptyInput)
  {
    _terminal.queueInput("\n");
    auto request = processAllInput();
    EXPECT_FALSE(request.has_value());
    EXPECT_TRUE(_terminal.getOutput().empty());
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

  TEST_F(InputParserTest, InvalidLoginFormat)
  {
    _currentState = CLIState::LoggedOut;
    _parser = std::make_unique<InputParser>(_terminal, _currentState);
    
    _terminal.queueInput("invalidformat\n");
    auto request = processAllInput();
    
    ASSERT_TRUE(request.has_value());
    EXPECT_NE(dynamic_cast<InvalidLoginRequest*>(request->get()), nullptr);
  }

  TEST_F(InputParserTest, EmptyLogin)
  {
    _currentState = CLIState::LoggedOut;
    _parser = std::make_unique<InputParser>(_terminal, _currentState);
    
    _terminal.queueInput("\n");
    auto request = processAllInput();
    
    EXPECT_FALSE(request.has_value());
  }

  TEST_F(InputParserTest, LoginWithEmptyFields)
  {
    _currentState = CLIState::LoggedOut;
    _parser = std::make_unique<InputParser>(_terminal, _currentState);
    
    // Test empty username
    _terminal.queueInput(":password\n");
    auto request = processAllInput();
    ASSERT_TRUE(request.has_value());
    EXPECT_NE(dynamic_cast<InvalidLoginRequest*>(request->get()), nullptr);
    
    // Test empty password
    _terminal.clearOutput();
    _terminal.queueInput("username:\n");
    request = processAllInput();
    ASSERT_TRUE(request.has_value());
    EXPECT_NE(dynamic_cast<InvalidLoginRequest*>(request->get()), nullptr);
  }

  TEST_F(InputParserTest, ArrowKeys)
  {
    // Test UP arrow
    _terminal.queueInput({0x1B, '[', 'A'});  // ESC [ A
    auto request = processAllInput();

    ASSERT_TRUE(request.has_value());
    auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::ArrowUp);

    // Test DOWN arrow
    _terminal.clearOutput();
    _terminal.queueInput({0x1B, '[', 'B'});  // ESC [ B
    request = processAllInput();

    ASSERT_TRUE(request.has_value());
    actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::ArrowDown);

    // Test LEFT arrow
    _terminal.clearOutput();
    _terminal.queueInput({0x1B, '[', 'D'});  // ESC [ D
    request = processAllInput();

    ASSERT_TRUE(request.has_value());
    actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::ArrowLeft);

    // Test RIGHT arrow
    _terminal.clearOutput();
    _terminal.queueInput({0x1B, '[', 'C'});  // ESC [ C
    request = processAllInput();

    ASSERT_TRUE(request.has_value());
    actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::ArrowRight);
  }

  TEST_F(InputParserTest, TabKey)
  {
    _terminal.queueInput(std::string(1, InputParser::TAB));
    auto request = processAllInput();

    ASSERT_TRUE(request.has_value());
    auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::Tab);
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

  TEST_F(InputParserTest, RootNavigation)
  {
    _terminal.queueInput("/\n");
    auto request = processAllInput();

    ASSERT_TRUE(request.has_value());
    auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_TRUE(actionRequest->getPath().isAbsolute());
    EXPECT_TRUE(actionRequest->getPath().elements().empty());
  }

  TEST_F(InputParserTest, HistoryNavigationWithoutPartialInput)
  {
    // Add commands to history
    _terminal.queueInput("command1\n");
    processAllInput();
    _terminal.queueInput("command2\n");
    processAllInput();

    _terminal.clearOutput();

    // Press Up Arrow with no partial input
    _terminal.queueInput({0x1B, '[', 'A'});  // ESC [ A
    auto request = processAllInput();

    ASSERT_TRUE(request.has_value());
    auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::ArrowUp);

    // In this case, we just directly show the command
    EXPECT_EQ(_terminal.getOutput(), "command2");
  }

  TEST_F(InputParserTest, SaveBufferDuringHistoryNavigation)
  {
    // Add commands to history
    _terminal.queueInput("command1\n");
    processAllInput();
    _terminal.queueInput("command2\n");
    processAllInput();

    // Start typing a new command
    _terminal.queueInput("new");
    processAllInput();

    _terminal.clearOutput();

    // Navigate up
    _terminal.queueInput({0x1B, '[', 'A'});  // Up arrow
    processAllInput();

    // Navigate down to restore buffer
    _terminal.queueInput({0x1B, '[', 'B'});  // Down arrow
    auto request = processAllInput();

    ASSERT_TRUE(request.has_value());
    auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::ArrowDown);

    std::string expectedOutput = generateClearSequence(3) +  // Clear "new"
                                "command2" +                 // Show command from history
                                generateClearSequence(8) +   // Clear command2
                                "new";                       // Show original input
    
    EXPECT_EQ(_terminal.getOutput(), expectedOutput);
  }

  TEST_F(InputParserTest, NavigatePastHistoryBounds)
  {
    // Add one command
    _terminal.queueInput("command1\n");
    processAllInput();

    _terminal.clearOutput();

    // Try to go up twice (should show same command both times)
    _terminal.queueInput({0x1B, '[', 'A'});  // Up
    auto request = processAllInput();
    EXPECT_EQ(_terminal.getOutput(), "command1");

    _terminal.clearOutput();
    _terminal.queueInput({0x1B, '[', 'A'});  // Up again
    request = processAllInput();
    std::string expectedOutput = generateClearSequence(8) + "command1";  // Clear "command1"
    EXPECT_EQ(_terminal.getOutput(), expectedOutput);  // Still shows same command

    // Try to go down twice
    _terminal.clearOutput();
    _terminal.queueInput({0x1B, '[', 'B'});  // Down
    request = processAllInput();
    EXPECT_EQ(_terminal.getOutput(), generateClearSequence(8));  // Clear "command1"

    _terminal.clearOutput();
    _terminal.queueInput({0x1B, '[', 'B'});  // Down again
    request = processAllInput();
    EXPECT_TRUE(_terminal.getOutput().empty());  // Already at bottom, no output

    ASSERT_TRUE(request.has_value());
    auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::ArrowDown);
  }

  TEST_F(InputParserTest, DuplicateCommandsInHistory)
  {
    // Add same command twice
    _terminal.queueInput("command1\n");
    processAllInput();
    _terminal.queueInput("command1\n");
    processAllInput();

    _terminal.clearOutput();

    // Navigate up twice
    _terminal.queueInput({0x1B, '[', 'A'});  // Up
    auto request = processAllInput();

    std::string expectedOutput = "command1";
    EXPECT_EQ(_terminal.getOutput(), expectedOutput);

    _terminal.clearOutput();
    _terminal.queueInput({0x1B, '[', 'A'});  // Up again
    request = processAllInput();

    expectedOutput = generateClearSequence(8) + "command1";  // Clear "command1" and rewrite "command1"
    EXPECT_EQ(_terminal.getOutput(), expectedOutput);

    ASSERT_TRUE(request.has_value());
    auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::ArrowUp);

    // Go back down
    _terminal.clearOutput();
    _terminal.queueInput({0x1B, '[', 'B'});  // Down
    request = processAllInput();
    EXPECT_EQ(_terminal.getOutput(), generateClearSequence(8));  // Only clear command since we're going back to empty
  }

}
