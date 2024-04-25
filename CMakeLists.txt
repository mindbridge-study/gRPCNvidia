cmake_minimum_required(VERSION 3.8)
project(gRPCNvidia LANGUAGES C CXX)

# Sets Fetch Content Directory
include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/_deps)
set(FETCHCONTENT_FULLY_DISCONNECTED OFF)

option(BUILD_TESTS_RPC "Build the tests." ON)
option(BUILD_LIB_RPC "Build the main library as an executable" ON)

# Set C++ Settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

# Compiler Optimizations
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Setup GRPC ________________________________________________________________________________
if(MSVC)
  add_definitions(-D_WIN32_WINNT=0x600)
endif()
find_package(Threads REQUIRED)

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
if(CMAKE_CROSSCOMPILING)
  find_program(_PROTOBUF_PROTOC protoc)
else()
  set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()

# Get GRPC
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP gRPC::grpc++)
if(CMAKE_CROSSCOMPILING)
  find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
  set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()
# End of Setup GRPC ________________________________________________________________________________


# Include Library ________________________________________________________________________________
include_directories(inc)
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.cc") # src files
# End of Include Library ________________________________________________________________________________


# Get Proto Files ________________________________________________________________________________
set (proto_name route_guide)
get_filename_component(hw_proto "${CMAKE_CURRENT_SOURCE_DIR}/proto/${proto_name}.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" DIRECTORY)

set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.h")

add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
           --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
           -I "${hw_proto_path}"
           --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
           "${hw_proto}"
      DEPENDS "${hw_proto}")

list(APPEND GENERATED_SOURCES ${hw_proto_srcs} ${hw_grpc_srcs})
list(APPEND GENERATED_HEADERS ${hw_proto_hdrs} ${hw_grpc_hdrs})

add_custom_target(GenerateProtos ALL
  DEPENDS "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}" "${GENERATED_SOURCES}" "${GENERATED_HEADERS}"
)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Append Library onto this
list(APPEND GENERATED_SOURCES ${SOURCES})

if(BUILD_LIB_RPC)
  add_executable(${PROJECT_NAME} ${GENERATED_SOURCES} ${GENERATED_HEADERS})
  target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
  target_link_libraries(${PROJECT_NAME} ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})
endif()
# End of Get Proto Files ________________________________________________________________________________


# Conditional Testing ________________________________________________________________________________
if(BUILD_TESTS_RPC)
  enable_testing()

  include(FetchContent)
  set(GOOGLETEST_DIR "${FETCHCONTENT_BASE_DIR}/googletest")
  
  if(NOT EXISTS "${GOOGLETEST_DIR}")
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    FetchContent_MakeAvailable(googletest)
  else()
    add_subdirectory("${GOOGLETEST_DIR}" "${CMAKE_BINARY_DIR}/googletest-build")
  endif()

  enable_testing()
  include(GoogleTest)

  file(GLOB_RECURSE TEST_SOURCES "tests/*.cc" "tests/*.cpp")
  add_executable(testexec ${TEST_SOURCES})
  add_dependencies(testexec GenerateProtos)

  target_link_libraries(testexec GTest::gtest_main ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

  gtest_discover_tests(testexec)
endif()
# End of Conditional Testing ________________________________________________________________________________
