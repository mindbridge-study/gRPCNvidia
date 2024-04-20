enable_testing()

file(GLOB_RECURSE TEST_SOURCES "*.cc" "*.cpp")
add_executable(testexec ${TEST_SOURCES})

target_link_libraries(testexec GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(testexec) # Finds tests inside test files
