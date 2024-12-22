# Common test settings
function(configure_test TEST_NAME)
  target_link_libraries(${TEST_NAME} 
    PRIVATE 
      GTest::GTest 
      GTest::Main
  )
  
  add_test(
    NAME ${TEST_NAME}
    COMMAND ${TEST_NAME}
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  )
  
  set_tests_properties(${TEST_NAME} 
    PROPERTIES 
    ENVIRONMENT "PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
  )
endfunction()


# ActionRequest tests
add_executable(${PROJECT_NAME}_ActionRequest_test
  tests/cli/ActionRequestTest.cpp
)

target_link_libraries(${PROJECT_NAME}_ActionRequest_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::gtest_main
)

# LoginRequest tests
add_executable(${PROJECT_NAME}_LoginRequest_test
  tests/cli/LoginRequestTest.cpp
)

target_link_libraries(${PROJECT_NAME}_LoginRequest_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::gtest_main
)

# InputParser tests
add_executable(${PROJECT_NAME}_InputParser_test
  tests/cli/InputParserTest.cpp
  mock/io/CharIOStreamMock.cpp
)

target_include_directories(${PROJECT_NAME}_InputParser_test
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}_InputParser_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::gtest_main
)

# Tree tests
add_executable(${PROJECT_NAME}_Tree_test
  tests/tree/TreeTest.cpp
)

target_link_libraries(${PROJECT_NAME}_Tree_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::gtest_main
)

# CLIService tests
add_executable(${PROJECT_NAME}_CLIService_test
  tests/cli/CLIServiceTest.cpp
  mock/io/CharIOStreamMock.cpp
  mock/command/CommandMock.hpp
)

target_include_directories(${PROJECT_NAME}_CLIService_test
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}_CLIService_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::gtest_main
    GTest::gmock
)

# Path tests
add_executable(${PROJECT_NAME}_Path_test
  tests/tree/PathTest.cpp
)

target_link_libraries(${PROJECT_NAME}_Path_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::gtest_main
)

# PathResolver tests
add_executable(${PROJECT_NAME}_PathResolver_test
  tests/tree/PathResolverTest.cpp
)

target_link_libraries(${PROJECT_NAME}_PathResolver_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::gtest_main
)

# PathCompleter tests
add_executable(${PROJECT_NAME}_PathCompleter_test
  tests/tree/PathCompleterTest.cpp
)

target_link_libraries(${PROJECT_NAME}_PathCompleter_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::gtest_main
)

# CommandHistory tests
add_executable(${PROJECT_NAME}_CommandHistory_test
  tests/cli/CommandHistoryTest.cpp
)

target_link_libraries(${PROJECT_NAME}_CommandHistory_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::gtest_main
)

# PathCompleter Advanced tests
add_executable(${PROJECT_NAME}_PathCompleterAdvanced_test
  tests/tree/PathCompleterAdvancedTest.cpp
)

target_link_libraries(${PROJECT_NAME}_PathCompleterAdvanced_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::gtest_main
)

# CLIService Advanced test
add_executable(${PROJECT_NAME}_CLIServiceAdvanced_test
  tests/cli/CLIServiceAdvancedTest.cpp
  mock/io/CharIOStreamMock.cpp
)

target_include_directories(${PROJECT_NAME}_CLIServiceAdvanced_test
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}_CLIServiceAdvanced_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::gtest_main
    GTest::gmock
)

# PathResolver Navigation tests
add_executable(${PROJECT_NAME}_PathResolverNavigation_test
  tests/tree/PathResolverNavigationTest.cpp
)

target_link_libraries(${PROJECT_NAME}_PathResolverNavigation_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::gtest_main
)

# TabCompletion tests
add_executable(${PROJECT_NAME}_TabCompletion_test
  tests/cli/TabCompletionTest.cpp
  mock/io/CharIOStreamMock.cpp
  mock/command/CommandMock.hpp
)

target_include_directories(${PROJECT_NAME}_TabCompletion_test
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}_TabCompletion_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::gtest_main
    GTest::gmock
)


# Configure tests
configure_test(${PROJECT_NAME}_ActionRequest_test)
configure_test(${PROJECT_NAME}_LoginRequest_test)
configure_test(${PROJECT_NAME}_InputParser_test)
configure_test(${PROJECT_NAME}_Tree_test)
configure_test(${PROJECT_NAME}_CLIService_test)
configure_test(${PROJECT_NAME}_Path_test)
configure_test(${PROJECT_NAME}_PathResolver_test)
configure_test(${PROJECT_NAME}_PathCompleter_test)
configure_test(${PROJECT_NAME}_CommandHistory_test)
configure_test(${PROJECT_NAME}_PathCompleterAdvanced_test)
configure_test(${PROJECT_NAME}_CLIServiceAdvanced_test)
configure_test(${PROJECT_NAME}_PathResolverNavigation_test)
configure_test(${PROJECT_NAME}_TabCompletion_test)


# Discover tests
gtest_discover_tests(${PROJECT_NAME}_ActionRequest_test
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  PROPERTIES TIMEOUT 10
)

gtest_discover_tests(${PROJECT_NAME}_LoginRequest_test
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  PROPERTIES TIMEOUT 10
)

gtest_discover_tests(${PROJECT_NAME}_InputParser_test
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  PROPERTIES TIMEOUT 10
)

gtest_discover_tests(${PROJECT_NAME}_Tree_test
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  PROPERTIES TIMEOUT 10
)

gtest_discover_tests(${PROJECT_NAME}_CLIService_test
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  PROPERTIES TIMEOUT 10
)

gtest_discover_tests(${PROJECT_NAME}_Path_test
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  PROPERTIES TIMEOUT 10
)

gtest_discover_tests(${PROJECT_NAME}_PathResolver_test
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  PROPERTIES TIMEOUT 10
)

gtest_discover_tests(${PROJECT_NAME}_PathCompleter_test
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  PROPERTIES TIMEOUT 10
)

gtest_discover_tests(${PROJECT_NAME}_CommandHistory_test
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  PROPERTIES TIMEOUT 10
)

gtest_discover_tests(${PROJECT_NAME}_PathCompleterAdvanced_test
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  PROPERTIES TIMEOUT 10
)

gtest_discover_tests(${PROJECT_NAME}_CLIServiceAdvanced_test
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  PROPERTIES TIMEOUT 10
)

gtest_discover_tests(${PROJECT_NAME}_PathResolverNavigation_test
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  PROPERTIES TIMEOUT 10
)

gtest_discover_tests(${PROJECT_NAME}_TabCompletion_test
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  PROPERTIES TIMEOUT 10
)
