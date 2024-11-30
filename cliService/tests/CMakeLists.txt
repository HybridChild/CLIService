# ActionRequest tests
add_executable(${PROJECT_NAME}_ActionRequest_test
  actionRequest/ActionRequestTest.cpp
)

target_link_libraries(${PROJECT_NAME}_ActionRequest_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::GTest
    GTest::Main
)

gtest_discover_tests(${PROJECT_NAME}_ActionRequest_test)

# Template for future components
# add_executable(${PROJECT_NAME}_<component>_test
#   <Component>Test.cpp
# )
# target_link_libraries(...)
# gtest_discover_tests(...)