cmake_minimum_required(VERSION 3.8)
project(gRPCNvidia LANGUAGES C CXX)

option(BUILD_PYTHON_LIBS_RPC "Build the Python bindings." OFF)
option(BUILD_LIB_RPC "Build the LIBRARY." OFF)
option(BUILD_SHARED_RPC "Build the LIBRARY AS SHARED." OFF)
option(BUILD_TESTS_RPC "Build the tests." ON)

if (BUILD_TESTS_RPC)
  enable_testing() # Enables Google Testing
endif()

# Set C++ Settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
  FetchContent_Declare(
    grpc
    GIT_REPOSITORY https://github.com/grpc/grpc.git
    # when using gRPC, you will actually set this to an existing tag, such as
    # v1.25.0, v1.26.0 etc..
    # For the purpose of testing, we override the tag used to the commit
    # that's currently under test.
    GIT_TAG        v1.62.1)
  FetchContent_MakeAvailable(grpc)

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
set(PYTHON_SOURCES ${SOURCES})
list(FILTER SOURCES EXCLUDE REGEX ".*extension\\.cc$") # Remove Exntesion because that Python Specific
# End of Include Library ________________________________________________________________________________


# Get Proto Files ________________________________________________________________________________
set(GENERATED_SOURCES "")
set(GENERATED_HEADERS "")
set(PROTO_PATH "./proto")
set(PROTO_NAME "Service")
file(GLOB PROTO_FILES "${PROTO_PATH}/*.proto")
foreach(PROTO_FILE IN LISTS PROTO_FILES)
  get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
  set(PROTO_SRC "${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.pb.cc")
  set(PROTO_HDR "${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.pb.h")
  set(GRPC_SRC "${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.grpc.pb.cc")
  set(GRPC_HDR "${CMAKE_CURRENT_BINARY_DIR}/${PROTO_NAME}.grpc.pb.h")

  add_custom_command(
    OUTPUT "${PROTO_SRC}" "${PROTO_HDR}" "${GRPC_SRC}" "${GRPC_HDR}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
         --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
         -I "${PROTO_PATH}"
         --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
         "${PROTO_FILE}"
    DEPENDS "${PROTO_FILE}"
  )
  list(APPEND GENERATED_SOURCES ${PROTO_SRC} ${GRPC_SRC})
  list(APPEND GENERATED_HEADERS ${PROTO_HDR} ${GRPC_HDR})
endforeach()

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Append Library onto this
list(APPEND PYTHON_SOURCES ${GENERATED_SOURCES})
list(APPEND GENERATED_SOURCES ${SOURCES})

set(LIB_TYPE STATIC)
if(BUILD_SHARED_RPC)
  set(LIB_TYPE SHARED)
endif()

if(BUILD_LIB_RPC)
  add_executable(${PROJECT_NAME} ${LIB_TYPE} ${GENERATED_SOURCES} ${GENERATED_HEADERS})
  target_link_libraries(${PROJECT_NAME} gRPC::grpc++ gRPC::grpc++_reflection protobuf::libprotobuf Threads::Threads)
endif()
# End of Get Proto Files ________________________________________________________________________________


# Python Specific Build Library ________________________________________________________________________________
if (BUILD_PYTHON_LIBS_RPC)
  set (Python_ADDITIONAL_VERSIONS 3)
  find_package (PythonLibs REQUIRED)
  find_package (PythonInterp REQUIRED)

  include_directories("${PYTHON_INCLUDE_DIRS}")
  list(FILTER SOURCES EXCLUDE REGEX ".*main\\.cc$") # Remove main because we dont want an executable

  add_library(core SHARED ${PYTHON_SOURCES})

  set_target_properties(core PROPERTIES PREFIX "" OUTPUT_NAME "gRPCNvidia")

  if (WIN32)
    set_target_properties(core PROPERTIES SUFFIX ".pyd")
  else()
    set_target_properties(core PROPERTIES SUFFIX ".so")
  endif()

  target_link_libraries(core "${PYTHON_LIBRARIES}") # Link Python Libraries
endif()
# End of Python Specific Build Library ________________________________________________________________________________


# Conditional Testing ________________________________________________________________________________
if(BUILD_TESTS_RPC)
  # Add GoogleTest
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  )

  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  # Add the tests directory
  add_subdirectory(tests)
endif()
# End of Conditional Testing ________________________________________________________________________________
