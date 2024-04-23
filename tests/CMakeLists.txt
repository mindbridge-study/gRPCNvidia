enable_testing()

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB_RECURSE TEST_SOURCES "*.cc" "*.cpp")
add_executable(testexec ${TEST_SOURCES})
add_dependencies(testexec GenerateProtos)

target_link_libraries(testexec GTest::gtest_main absl::flags absl::flags_parse absl::strings ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

include(GoogleTest)
gtest_discover_tests(testexec) # Finds tests inside test files
