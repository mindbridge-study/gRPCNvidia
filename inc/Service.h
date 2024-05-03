#pragma once

#include <cassert>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <grpcpp/grpcpp.h>

#include "route_guide.grpc.pb.h"

using grpc::Server;
using grpc::ServerAsyncReader;
using grpc::ServerAsyncReaderWriter;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerAsyncWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
using routeguide::ImageChunk;
using routeguide::PingRequest;
using routeguide::PingResponse;
using routeguide::RouteGuide;
using routeguide::UploadStatus;
using grpc::experimental::InterceptionHookPoints;
using grpc::experimental::Interceptor;
using grpc::experimental::InterceptorBatchMethods;
using grpc::experimental::ServerInterceptorFactoryInterface;
using grpc::experimental::ServerRpcInfo;

/*
 * Dependency of Server Implementation that will be used to handle RPCs, by detecting their type and routing them asyncrhonously
 */
class CallData {
public:
  enum CallStatus { CREATE, PROCESS, FINISH };
  enum Type { UPLOAD_IMAGE, PING };

private:
  // Status of the Call and Type
  CallStatus status_;
  Type type_;

  // Service, Queue, and Context
  RouteGuide::AsyncService *service_;
  ServerCompletionQueue *cq_;
  ServerContext ctx_;

  // Reader/Writers
  ServerAsyncReader<UploadStatus, ImageChunk> stream_;
  ServerAsyncResponseWriter<PingResponse> responder_;

  // Proto Objects
  ImageChunk chunk; // might be a nullptr even though address is sent so idk
  PingRequest ping_request;
  PingResponse ping_response;

public:
  /*
   *
   */
  CallData(RouteGuide::AsyncService *service, ServerCompletionQueue *cq,
           Type type);

  // move and copy constructors and operators
  CallData(const CallData &) = delete;
  CallData &operator=(const CallData &) = delete;
  CallData(CallData &&) = delete;
  CallData &operator=(CallData &&) = delete;

  /*
   *
   */
  void Proceed();

private:
  inline void UploadImage();
  inline void Ping();
};

/*
 * The Server Implementation, which handles all the RPCs
 */
class ServerImpl final {
private:
  std::unique_ptr<ServerCompletionQueue> cq_;
  RouteGuide::AsyncService service_;
  std::unique_ptr<Server> server_;

public:
  explicit ServerImpl();
  ~ServerImpl();

  // move and copy constructors and operators
  ServerImpl(const ServerImpl &) = delete;
  ServerImpl &operator=(const ServerImpl &) = delete;
  ServerImpl(ServerImpl &&) = delete;
  ServerImpl &operator=(ServerImpl &&) = delete;

  /*
   *
   */
  void Run(uint16_t port);

private:
  /*
   * Starts an Event loop to handle all incoming RPCs
   */
  void HandleRpcs();
};
