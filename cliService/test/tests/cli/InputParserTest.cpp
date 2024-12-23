#include "cliService/cli/InputParser.hpp"
#include "cliService/cli/ActionRequest.hpp"
#include "cliService/cli/LoginRequest.hpp"
#include "mock/io/CharIOStreamMock.hpp"
#include <gtest/gtest.h>

namespace cliService
{

  class InputParserTest : public ::testing::Test
  {
    static constexpr size_t HISTORY_SIZE = 10;
    static constexpr uint32_t INPUT_TIMEOUT_MS = 1000;

  protected:
    void SetUp() override
    {
      _currentState = CLIState::LoggedIn;
      _inputParser = std::make_unique<InputParser>(_ioStream, _currentState, INPUT_TIMEOUT_MS, HISTORY_SIZE);
    }

    CharIOStreamMock _ioStream;
    CLIState _currentState;
    std::unique_ptr<InputParser> _inputParser;

    // Helper to process all queued input
    std::optional<std::unique_ptr<RequestBase>> processAllInput()
    {
      std::optional<std::unique_ptr<RequestBase>> request;

      while (_ioStream.available()) {
        request = _inputParser->getNextRequest();
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
    _ioStream.queueInput("\n");
    auto request = processAllInput();
    EXPECT_FALSE(request.has_value());
    EXPECT_TRUE(_ioStream.getOutput().empty());
  }

  TEST_F(InputParserTest, SimpleCommand)
  {
    _ioStream.queueInput("command\n");
    auto request = processAllInput();

    ASSERT_TRUE(request.has_value());
    auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_EQ(actionRequest->getPath().elements()[0], "command");
    EXPECT_EQ(_ioStream.getOutput(), "command\r\n");
  }

  TEST_F(InputParserTest, CommandWithArguments)
  {
    _ioStream.queueInput("command arg1 arg2\n");
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
    _ioStream.queueInput("commanf");
    processAllInput();
    EXPECT_EQ(_ioStream.getOutput(), "commanf");

    _ioStream.clearOutput();
    _ioStream.queueInput(std::string(1, InputParser::BACKSPACE_BS));
    processAllInput();
    EXPECT_EQ(_ioStream.getOutput(), "\b \b");

    _ioStream.clearOutput();
    _ioStream.queueInput("d\n");
    auto request = processAllInput();
    EXPECT_EQ(_ioStream.getOutput(), "d\r\n");

    ASSERT_TRUE(request.has_value());
    auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_EQ(actionRequest->getPath().elements()[0], "command");
  }

  TEST_F(InputParserTest, LoginHandling)
  {
    _currentState = CLIState::LoggedOut;

    _ioStream.queueInput("user:pass\n");
    auto request = processAllInput();

    ASSERT_TRUE(request.has_value());
    auto* loginRequest = dynamic_cast<LoginRequest*>(request.value().get());
    ASSERT_NE(loginRequest, nullptr);
    EXPECT_EQ(loginRequest->getUsername(), "user");
    EXPECT_EQ(loginRequest->getPassword(), "pass");

    // Verify password masking
    std::string expectedOutput = "user:****\r\n";
    EXPECT_EQ(_ioStream.getOutput(), expectedOutput);
  }

  TEST_F(InputParserTest, InvalidLoginFormat)
  {
    _currentState = CLIState::LoggedOut;
    
    _ioStream.queueInput("invalidformat\n");
    auto request = processAllInput();
    
    ASSERT_TRUE(request.has_value());
    EXPECT_NE(dynamic_cast<InvalidLoginRequest*>(request->get()), nullptr);
  }

  TEST_F(InputParserTest, EmptyLogin)
  {
    _currentState = CLIState::LoggedOut;
    
    _ioStream.queueInput("\n");
    auto request = processAllInput();
    
    EXPECT_FALSE(request.has_value());
  }

  TEST_F(InputParserTest, LoginWithEmptyFields)
  {
    _currentState = CLIState::LoggedOut;
    
    // Test empty username
    _ioStream.queueInput(":password\n");
    auto request = processAllInput();
    ASSERT_TRUE(request.has_value());
    EXPECT_NE(dynamic_cast<InvalidLoginRequest*>(request->get()), nullptr);
    
    // Test empty password
    _ioStream.clearOutput();
    _ioStream.queueInput("username:\n");
    request = processAllInput();
    ASSERT_TRUE(request.has_value());
    EXPECT_NE(dynamic_cast<InvalidLoginRequest*>(request->get()), nullptr);
  }

  TEST_F(InputParserTest, ArrowKeys)
  {
    // Test UP arrow
    _ioStream.queueInput({0x1B, '[', 'A'});  // ESC [ A
    auto request = processAllInput();

    ASSERT_TRUE(request.has_value());
    auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::ArrowUp);

    // Test DOWN arrow
    _ioStream.clearOutput();
    _ioStream.queueInput({0x1B, '[', 'B'});  // ESC [ B
    request = processAllInput();

    ASSERT_TRUE(request.has_value());
    actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::ArrowDown);
  }

  TEST_F(InputParserTest, TabKey)
  {
    _ioStream.queueInput(std::string(1, InputParser::TAB));
    auto request = processAllInput();

    ASSERT_TRUE(request.has_value());
    auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::Tab);
  }

  TEST_F(InputParserTest, MultipleBackspace)
  {
    _ioStream.queueInput("hello");
    processAllInput();
    EXPECT_EQ(_ioStream.getOutput(), "hello");

    _ioStream.clearOutput();
    _ioStream.queueInput(std::string(1, InputParser::BACKSPACE_BS));
    processAllInput();
    EXPECT_EQ(_ioStream.getOutput(), "\b \b");

    _ioStream.clearOutput();
    _ioStream.queueInput(std::string(1, InputParser::BACKSPACE_BS));
    processAllInput();
    EXPECT_EQ(_ioStream.getOutput(), "\b \b");

    _ioStream.clearOutput();
    _ioStream.queueInput("\n");
    auto request = processAllInput();

    ASSERT_TRUE(request.has_value());
    auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_EQ(actionRequest->getPath().elements()[0], "hel");
  }

  TEST_F(InputParserTest, RootNavigation)
  {
    _ioStream.queueInput("/\n");
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
    _ioStream.queueInput("command1\n");
    processAllInput();
    _ioStream.queueInput("command2\n");
    processAllInput();

    _ioStream.clearOutput();

    // Press Up Arrow with no partial input
    _ioStream.queueInput({0x1B, '[', 'A'});  // ESC [ A
    auto request = processAllInput();

    ASSERT_TRUE(request.has_value());
    auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::ArrowUp);

    // In this case, we just directly show the command
    EXPECT_EQ(_ioStream.getOutput(), "command2");
  }

  TEST_F(InputParserTest, SaveBufferDuringHistoryNavigation)
  {
    // Add commands to history
    _ioStream.queueInput("command1\n");
    processAllInput();
    _ioStream.queueInput("command2\n");
    processAllInput();

    // Start typing a new command
    _ioStream.queueInput("new");
    processAllInput();

    _ioStream.clearOutput();

    // Navigate up
    _ioStream.queueInput({0x1B, '[', 'A'});  // Up arrow
    processAllInput();

    // Navigate down to restore buffer
    _ioStream.queueInput({0x1B, '[', 'B'});  // Down arrow
    auto request = processAllInput();

    ASSERT_TRUE(request.has_value());
    auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::ArrowDown);

    std::string expectedOutput = generateClearSequence(3) +  // Clear "new"
                                "command2" +                 // Show command from history
                                generateClearSequence(8) +   // Clear command2
                                "new";                       // Show original input
    
    EXPECT_EQ(_ioStream.getOutput(), expectedOutput);
  }

  TEST_F(InputParserTest, NavigatePastHistoryBounds)
  {
    // Add one command
    _ioStream.queueInput("command1\n");
    processAllInput();

    _ioStream.clearOutput();

    // Try to go up twice (should show same command both times)
    _ioStream.queueInput({0x1B, '[', 'A'});  // Up
    auto request = processAllInput();
    EXPECT_EQ(_ioStream.getOutput(), "command1");

    _ioStream.clearOutput();
    _ioStream.queueInput({0x1B, '[', 'A'});  // Up again
    request = processAllInput();
    std::string expectedOutput = generateClearSequence(8) + "command1";  // Clear "command1"
    EXPECT_EQ(_ioStream.getOutput(), expectedOutput);  // Still shows same command

    // Try to go down twice
    _ioStream.clearOutput();
    _ioStream.queueInput({0x1B, '[', 'B'});  // Down
    request = processAllInput();
    EXPECT_EQ(_ioStream.getOutput(), generateClearSequence(8));  // Clear "command1"

    _ioStream.clearOutput();
    _ioStream.queueInput({0x1B, '[', 'B'});  // Down again
    request = processAllInput();
    EXPECT_TRUE(_ioStream.getOutput().empty());  // Already at bottom, no output

    ASSERT_TRUE(request.has_value());
    auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::ArrowDown);
  }

  TEST_F(InputParserTest, DuplicateCommandsInHistory)
  {
    // Add same command twice
    _ioStream.queueInput("command1\n");
    processAllInput();
    _ioStream.queueInput("command1\n");
    processAllInput();

    _ioStream.clearOutput();

    // Navigate up twice
    _ioStream.queueInput({0x1B, '[', 'A'});  // Up
    auto request = processAllInput();

    std::string expectedOutput = "command1";
    EXPECT_EQ(_ioStream.getOutput(), expectedOutput);

    _ioStream.clearOutput();
    _ioStream.queueInput({0x1B, '[', 'A'});  // Up again
    request = processAllInput();

    expectedOutput = generateClearSequence(8) + "command1";  // Clear "command1" and rewrite "command1"
    EXPECT_EQ(_ioStream.getOutput(), expectedOutput);

    ASSERT_TRUE(request.has_value());
    auto* actionRequest = dynamic_cast<ActionRequest*>(request.value().get());
    ASSERT_NE(actionRequest, nullptr);
    EXPECT_EQ(actionRequest->getTrigger(), ActionRequest::Trigger::ArrowUp);

    // Go back down
    _ioStream.clearOutput();
    _ioStream.queueInput({0x1B, '[', 'B'});  // Down
    request = processAllInput();
    EXPECT_EQ(_ioStream.getOutput(), generateClearSequence(8));  // Only clear command since we're going back to empty
  }

}
