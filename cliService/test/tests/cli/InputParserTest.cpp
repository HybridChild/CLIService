#include "cliService/cli/InputParser.hpp"
#include "cliService/cli/CommandRequest.hpp"
#include "cliService/cli/TabCompletionRequest.hpp"
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
      _inputParser = std::make_unique<InputParser>(_ioStream, _currentState, INPUT_TIMEOUT_MS);
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
    auto* commandRequest = dynamic_cast<CommandRequest*>(request.value().get());
    ASSERT_NE(commandRequest, nullptr);
    EXPECT_EQ(commandRequest->getPath().elements()[0], "command");
    EXPECT_EQ(_ioStream.getOutput(), "command\r\n");
  }

  TEST_F(InputParserTest, CommandWithArguments)
  {
    _ioStream.queueInput("command arg1 arg2\n");
    auto request = processAllInput();

    ASSERT_TRUE(request.has_value());
    auto* commandRequest = dynamic_cast<CommandRequest*>(request.value().get());
    ASSERT_NE(commandRequest, nullptr);
    EXPECT_EQ(commandRequest->getPath().elements()[0], "command");
    ASSERT_EQ(commandRequest->getArgs().size(), 2);
    EXPECT_EQ(commandRequest->getArgs()[0], "arg1");
    EXPECT_EQ(commandRequest->getArgs()[1], "arg2");
  }

  TEST_F(InputParserTest, TabCompletion)
  {
    _ioStream.queueInput("command");
    processAllInput();
    _ioStream.clearOutput();
    
    _ioStream.queueInput(std::string(1, InputParser::TAB));
    auto request = processAllInput();

    ASSERT_TRUE(request.has_value());
    auto* tabRequest = dynamic_cast<TabCompletionRequest*>(request.value().get());
    ASSERT_NE(tabRequest, nullptr);
    EXPECT_EQ(tabRequest->getPath().elements()[0], "command");
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
    auto* commandRequest = dynamic_cast<CommandRequest*>(request.value().get());
    ASSERT_NE(commandRequest, nullptr);
    EXPECT_EQ(commandRequest->getPath().elements()[0], "command");
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

  TEST_F(InputParserTest, LoginParsingEmptyInput)
  {
    _currentState = CLIState::LoggedOut;
    _ioStream.queueInput("\n");
    auto request = processAllInput();
    EXPECT_FALSE(request.has_value());
  }

  TEST_F(InputParserTest, LoginParsingEmptyFields)
  {
    _currentState = CLIState::LoggedOut;
    
    // Empty username
    _ioStream.queueInput(":pass\n");
    auto request = processAllInput();
    ASSERT_TRUE(request.has_value());
    EXPECT_NE(dynamic_cast<InvalidLoginRequest*>(request->get()), nullptr);
    
    // Empty password
    _ioStream.queueInput("user:\n");
    request = processAllInput();
    ASSERT_TRUE(request.has_value());
    EXPECT_NE(dynamic_cast<InvalidLoginRequest*>(request->get()), nullptr);
  }

  TEST_F(InputParserTest, LoginParsingComplexInput)
  {
    _currentState = CLIState::LoggedOut;
    
    // Multiple colons
    _ioStream.queueInput("user:pass:extra\n");
    auto request = processAllInput();
    ASSERT_TRUE(request.has_value());
    auto* loginRequest = dynamic_cast<LoginRequest*>(request->get());
    ASSERT_NE(loginRequest, nullptr);
    EXPECT_EQ(loginRequest->getUsername(), "user");
    EXPECT_EQ(loginRequest->getPassword(), "pass:extra");
    
    // Special characters
    _ioStream.queueInput("user@domain.com:P@ssw0rd!\n");
    request = processAllInput();
    ASSERT_TRUE(request.has_value());
    loginRequest = dynamic_cast<LoginRequest*>(request->get());
    ASSERT_NE(loginRequest, nullptr);
    EXPECT_EQ(loginRequest->getUsername(), "user@domain.com");
    EXPECT_EQ(loginRequest->getPassword(), "P@ssw0rd!");
  }

  TEST_F(InputParserTest, InvalidLoginFormat)
  {
    _currentState = CLIState::LoggedOut;
    
    _ioStream.queueInput("invalidformat\n");
    auto request = processAllInput();
    
    ASSERT_TRUE(request.has_value());
    EXPECT_NE(dynamic_cast<InvalidLoginRequest*>(request->get()), nullptr);
  }

  TEST_F(InputParserTest, ComplexPathHandling)
  {
    _ioStream.queueInput("dir1/../../dir2/command arg1\n");
    auto request = processAllInput();

    ASSERT_TRUE(request.has_value());
    auto* commandRequest = dynamic_cast<CommandRequest*>(request.value().get());
    ASSERT_NE(commandRequest, nullptr);
    
    const auto& elements = commandRequest->getPath().elements();
    ASSERT_EQ(elements.size(), 5);
    EXPECT_EQ(elements[0], "dir1");
    EXPECT_EQ(elements[1], "..");
    EXPECT_EQ(elements[2], "..");
    EXPECT_EQ(elements[3], "dir2");
    EXPECT_EQ(elements[4], "command");
    
    ASSERT_EQ(commandRequest->getArgs().size(), 1);
    EXPECT_EQ(commandRequest->getArgs()[0], "arg1");
  }

  TEST_F(InputParserTest, BufferManagementBasic)
  {
    _inputParser->replaceBuffer("test");
    EXPECT_EQ(_inputParser->getBuffer(), "test");
    EXPECT_EQ(_ioStream.getOutput(), "test");

    _ioStream.clearOutput();
    _inputParser->replaceBuffer("new");
    // Verify both buffer clear and new content
    EXPECT_EQ(_ioStream.getOutput(), "\b \b\b \b\b \b\b \bnew");
    EXPECT_EQ(_inputParser->getBuffer(), "new");
  }

  TEST_F(InputParserTest, BufferAppending)
  {
    _inputParser->replaceBuffer("base");
    _ioStream.clearOutput();
    
    _inputParser->appendToBuffer(" extra");
    EXPECT_EQ(_inputParser->getBuffer(), "base extra");
    EXPECT_EQ(_ioStream.getOutput(), " extra");
  }

  TEST_F(InputParserTest, HistoryNavigationRequestGeneration)
  {
    _ioStream.queueInput({0x1B, '[', 'A'}); // Up arrow
    auto request = processAllInput();
    
    ASSERT_TRUE(request.has_value());
    auto* historyRequest = dynamic_cast<HistoryNavigationRequest*>(request.value().get());
    ASSERT_NE(historyRequest, nullptr);
    EXPECT_EQ(historyRequest->getDirection(), HistoryNavigationRequest::Direction::Previous);
    EXPECT_EQ(historyRequest->getCurrentBuffer(), "");
  }

}
