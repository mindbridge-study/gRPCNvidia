enable_testing()

file(GLOB_RECURSE TEST_SOURCES "*.cc" "*.cpp")
add_executable(testexec ${TEST_SOURCES})

target_link_libraries(testexec GTest::gtest_main absl::flags absl::flags_parse absl::strings ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

include(GoogleTest)
gtest_discover_tests(testexec) # Finds tests inside test files
