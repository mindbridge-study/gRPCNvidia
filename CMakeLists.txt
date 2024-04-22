cmake_minimum_required(VERSION 3.8)
project(gRPCNvidia LANGUAGES C CXX)

# Sets Fetch Content Directory
include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/_deps)
set(FETCHCONTENT_FULLY_DISCONNECTED OFF)

option(BUILD_TESTS_RPC "Build the tests." ON)
option(BUILD_LIB_RPC "Build the main library as an executable" ON)

if (BUILD_TESTS_RPC)
  enable_testing() # Enables Google Testing
endif()

# Set C++ Settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

message(STATUS "Using gRPC via add_subdirectory (FetchContent).")
include(FetchContent)
set(ABSL_ENABLE_INSTALL ON)

if(NOT EXISTS "${FETCHCONTENT_BASE_DIR}/grpc")
  FetchContent_Declare(
      grpc
      GIT_REPOSITORY https://github.com/grpc/grpc.git
      GIT_TAG        v1.62.1
  )
  FetchContent_MakeAvailable(grpc)
endif()

# Since FetchContent uses add_subdirectory under the hood, we can use
# the grpc targets directly from this build.
set(_PROTOBUF_LIBPROTOBUF libprotobuf)
set(_REFLECTION grpc++_reflection)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
set(_GRPC_GRPCPP grpc++)
if(CMAKE_CROSSCOMPILING)
  find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
  set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
endif()
# End of Setup GRPC ________________________________________________________________________________


# Include Library ________________________________________________________________________________
include_directories(inc)
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.cc") # src files
# End of Include Library ________________________________________________________________________________


# Get Proto Files ________________________________________________________________________________
get_filename_component(hw_proto "${CMAKE_CURRENT_SOURCE_DIR}/proto/services.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" DIRECTORY)

set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/services.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/services.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/services.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/services.grpc.pb.h")

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

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Append Library onto this
list(APPEND GENERATED_SOURCES ${SOURCES})

if(BUILD_LIB_RPC)
  add_executable(${PROJECT_NAME} ${GENERATED_SOURCES} ${GENERATED_HEADERS})
  target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
  target_link_libraries(${PROJECT_NAME} absl::flags absl::flags_parse absl::strings ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})
endif()
# End of Get Proto Files ________________________________________________________________________________


# Conditional Testing ________________________________________________________________________________
if(BUILD_TESTS_RPC)
  # Add GoogleTest
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

  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  # Add the tests directory
  add_subdirectory(tests)
endif()
# End of Conditional Testing ________________________________________________________________________________
