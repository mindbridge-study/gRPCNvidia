#pragma once

#include <grpc/grpc.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include <iostream>

#include "route_guide.grpc.pb.h"

class ServiceImpl : public routeguide::RouteGuide::AsyncService {
public:
  explicit ServiceImpl() = default;
  ~ServiceImpl();

  // Deleted constructors and operators for copy and move
  ServiceImpl(const ServiceImpl&) = delete;
  ServiceImpl& operator=(const ServiceImpl&) = delete;
  ServiceImpl(ServiceImpl&&) = delete;
  ServiceImpl& operator=(ServiceImpl&&) = delete;



public:
  /* Examples
  grpc::Status SimpleRPC(grpc::ServerContext* context, const routeguide::Point* point, routeguide::Feature* feature) override;
  grpc::Status ServerStreaming(grpc::ServerContext* context, const grpc::ServerReader<routeguide::Point>* reader, routeguide::RouteSummary* summary) override;
  grpc::Status ClientStreaming(grpc::ServerContext* context, grpc::ServerReaderWriter<routeguide::Feature, routeguide::Point>* stream) override;
  grpc::Status BidirectionalStream(grpc::ServerContext* context, grpc::ServerReaderWriter<routeguide::Feature, routeguide::Point>* stream) override;
  */

  grpc::Status UploadImage(grpc::ServerContext* context, const grpc::ServerReader<routeguide::ImageChunk> *reader, routeguide::UploadStatus * status) override;

  grpc::Status BidirectionalImageTransfer(grpc::ServerContext* context, grpc::ServerReaderWriter<routeguide::ImageChunk, routeguide::ImageChunk>* stream) override;

  void Run();

private:
  std::mutex mux_;
  std::unique_ptr<grpc::ServerCompletionQueue> _queue;
  std::unique_ptr<grpc::Server> _server;

  class CallData {
  public:
    CallData(routeguide::RouteGuide::AsyncService* service, grpc::ServerCompletionQueue* queue)
      : _service(service), _queue(queue), _responder(&_context), _status(CallStatus::CREATE) {
      Proceed();
    }

    void Proceed() {
      switch (_status) {
        case CallStatus::CREATE: {
          _status = CallStatus::PROCESS;
          _service->RequestUploadImage(&_context, &_reader, &_responder, _queue, _queue, this);
          break;
        }
        case CallStatus::PROCESS: {
          new CallData(_service, _queue); // Prepare to handle the next incoming request
          ImageChunk chunk;
          while (_reader.Read(&chunk)) {
              // Process each chunk; Example: Store it or assemble it
          }
          _status = CallStatus::FINISH;
          _upload_status.set_success(true);
          _upload_status.set_message("Upload completed successfully");
          _responder.Finish(_upload_status, Status::OK, this);
          break;
        }
        default: {
          delete this;
        }
      }
    }

  private:
    routeguide::RouteGuide::AsyncService* _service;
    grpc::ServerCompletionQueue* _queue;
    grpc::ServerContext _context;
    grpc::ServerAsyncReader<routeguide::UploadStatus, ImageChunk> _reader;
    routeguide::UploadStatus _upload_status;
    grpc::ServerAsyncResponseWriter<routeguide::UploadStatus> _responder;
    enum class CallStatus { CREATE, PROCESS, FINISH };
    CallStatus _status;
  };

  void HandleRPCs() {
    new CallData(this, _queue.get()); // Create initial CallData for handling UploadImage
    void* tag; // Holds a pointer to a CallData instance
    bool ok;
    while (true) {
      GPR_ASSERT(_queue->Next(&tag, &ok));
      GPR_ASSERT(ok); // Ensure the operation completed successfully
      static_cast<CallData*>(tag)->Proceed();
    }
  }
};
