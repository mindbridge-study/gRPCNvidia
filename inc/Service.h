#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <cassert>

#include <grpcpp/grpcpp.h>

#include "route_guide.grpc.pb.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
using grpc::ServerAsyncReader;
using grpc::ServerAsyncWriter;
using grpc::ServerAsyncReaderWriter;
using routeguide::RouteGuide;
using routeguide::ImageChunk;
using routeguide::UploadStatus;

class ServerImpl final {
public:
  friend class CallData;

private:
  std::unique_ptr<ServerCompletionQueue> cq_;
  RouteGuide::AsyncService service_;
  std::unique_ptr<Server> server_;

public:
  explicit ServerImpl();
  ~ServerImpl();

  // move and copy constructors and operators
  ServerImpl(const ServerImpl&) = delete;
  ServerImpl& operator=(const ServerImpl&) = delete;
  ServerImpl(ServerImpl&&) = delete;
  ServerImpl& operator=(ServerImpl&&) = delete;

  /*
   *
   */
  void Run(uint16_t port);
private:
  /*
   *
   */
  void HandleRpcs();
};



class CallData {
public:
  enum CallStatus { CREATE, PROCESS, FINISH };
  enum Type { UPLOAD_IMAGE, BIDIRECTIONAL_TRANSFER };

private:
  RouteGuide::AsyncService* service_;
  ServerCompletionQueue* cq_;
  ServerContext ctx_;
  CallStatus status_;
  Type type_;
  ServerAsyncReader<UploadStatus, ImageChunk> stream_;
  ServerAsyncReaderWriter<ImageChunk, ImageChunk> bidi_stream_;
  ServerImpl* server_;
  ImageChunk chunk;

public:
  /*
   *
   */
  CallData(RouteGuide::AsyncService* service, ServerCompletionQueue* cq, Type type, ServerImpl* server);
  ~CallData();

  // move and copy constructors and operators
  CallData(const CallData&) = delete;
  CallData& operator=(const CallData&) = delete;
  CallData(CallData&&) = delete;
  CallData& operator=(CallData&&) = delete;

  /*
   *
   */
  void Proceed();
};
