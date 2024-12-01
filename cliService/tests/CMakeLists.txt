# ActionRequest tests
add_executable(${PROJECT_NAME}_ActionRequest_test
  io/ActionRequestTest.cpp
)

target_link_libraries(${PROJECT_NAME}_ActionRequest_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::GTest
    GTest::Main
)

# Tree tests
add_executable(${PROJECT_NAME}_Tree_test
  tree/TreeTest.cpp
)

target_include_directories(${PROJECT_NAME}_Tree_test
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/tree
)

target_link_libraries(${PROJECT_NAME}_Tree_test
  PRIVATE
    ${PROJECT_NAME}_lib
    GTest::GTest
    GTest::Main
)

gtest_discover_tests(${PROJECT_NAME}_ActionRequest_test)
gtest_discover_tests(${PROJECT_NAME}_Tree_test)
