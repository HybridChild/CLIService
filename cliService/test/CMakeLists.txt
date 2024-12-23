# List of tests (just source file)
set(cli_tests
  Tree_test:tests/tree/TreeTest.cpp
  Path_test:tests/tree/PathTest.cpp
  PathResolver_test:tests/tree/PathResolverTest.cpp
  PathCompleter_test:tests/tree/PathCompleterTest.cpp
  CommandHistory_test:tests/cli/CommandHistoryTest.cpp
  PathCompleterAdvanced_test:tests/tree/PathCompleterAdvancedTest.cpp
  PathResolverNavigation_test:tests/tree/PathResolverNavigationTest.cpp
  CLIService_test:tests/cli/CLIServiceTest.cpp
  TabCompletion_test:tests/cli/TabCompletionTest.cpp
  InputParser_test:tests/cli/InputParserTest.cpp
)

# Configure tests with both mocks
foreach(test_info IN LISTS cli_tests)
  string(REPLACE ":" ";" test_parts "${test_info}")
  list(GET test_parts 0 test_name)
  list(GET test_parts 1 test_source)
  
  add_executable(${PROJECT_NAME}_${test_name} 
    ${test_source}
    mock/io/CharIOStreamMock.cpp
    mock/command/CommandMock.hpp
  )
  target_include_directories(${PROJECT_NAME}_${test_name}
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}
  )
  target_link_libraries(${PROJECT_NAME}_${test_name} 
    PRIVATE 
      ${PROJECT_NAME}_lib
      GTest::gtest_main
      GTest::gmock
  )
  add_test(
    NAME ${PROJECT_NAME}_${test_name}
    COMMAND ${PROJECT_NAME}_${test_name}
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  )
  set_tests_properties(${PROJECT_NAME}_${test_name}
    PROPERTIES 
    ENVIRONMENT "PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
  )
  gtest_discover_tests(${PROJECT_NAME}_${test_name}
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    PROPERTIES TIMEOUT 10
  )
endforeach()
