#include "Service.h"

ServerImpl::ServerImpl() {
  // On Service Start
  
  // @TODO!: Implement Python IPC Interaction for PostProcessing
}

ServerImpl::~ServerImpl() {
  // On Service Stop
  this->server_->Shutdown();
  this->cq_->Shutdown();
}

void ServerImpl::Run(uint16_t port) {
  std::string server_address = "0.0.0.0:" + std::to_string(port);
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&this->service_);
  this->cq_ = builder.AddCompletionQueue();
  this->server_ = builder.BuildAndStart();
  std::cout << "Server listening on " << server_address << std::endl;
  HandleRpcs();
}

void ServerImpl::HandleRpcs() {
  // Create Methods that the function needs
  new CallData(&this->service_, this->cq_.get(), CallData::UPLOAD_IMAGE, this);
  new CallData(&this->service_, this->cq_.get(), CallData::BIDIRECTIONAL_TRANSFER, this);

  void* tag;
  bool ok;
  while (true) {
    assert(this->cq_->Next(&tag, &ok));
    assert(ok);
    static_cast<CallData*>(tag)->Proceed();
  }
}

CallData::CallData(RouteGuide::AsyncService* service, ServerCompletionQueue* cq, Type type, ServerImpl* server)
  : service_(service), cq_(cq), type_(type), server_(server), status_(CREATE), stream_(&ctx_), bidi_stream_(&ctx_) {
  Proceed();
}

CallData::~CallData() {
  // Finish When the Task is Done
  
}


void CallData::Proceed() {
  if (status_ == CREATE) { // Request Created
      status_ = PROCESS;
      if (type_ == UPLOAD_IMAGE) {
          service_->RequestUploadImage(&ctx_, &stream_, cq_, cq_, this);
          stream_.Read(&chunk, this);  // Start reading data
      } else if (type_ == BIDIRECTIONAL_TRANSFER) {
          service_->RequestBidirectionalImageTransfer(&ctx_, &bidi_stream_, cq_, cq_, this);
          bidi_stream_.Read(&chunk, this);  // Start reading data
      }
  } else if (status_ == PROCESS) { // Request Processing
      if (type_ == UPLOAD_IMAGE) {
          stream_.Read(&chunk, this);  // Continue reading next chunk
      } else if (type_ == BIDIRECTIONAL_TRANSFER) {
          bidi_stream_.Write(chunk, this);  // Respond back with the chunk
          bidi_stream_.Read(&chunk, this);  // Continue reading
      }
  } else if (status_ == FINISH) {
      if (type_ == UPLOAD_IMAGE) {
          UploadStatus final_message;
          final_message.set_success(true);
          final_message.set_message("Upload completed successfully");
          stream_.Finish(final_message, Status::OK, this);
      } else {
          // Since Finish on ServerAsyncReaderWriter does not take a message
          // Send any final data before calling Finish
          bidi_stream_.Write(chunk, this);  // Send the final message, if necessary
          bidi_stream_.Finish(Status::OK, this);  // Correctly call Finish with two arguments
      }
  } else {
      delete this;  // Clean up the CallData instance
  }
}

/* Examples
grpc::Status ServiceImpl::SimpleRPC(grpc::ServerContext* context, const routeguide::Point* point, routeguide::Feature* feature) override {
  auto start = std::chrono::high_resolution_clock::now();

  // Processing
  feature->set_name("SimpleRPC");
  // End of Processing

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> elapsed = end - start;
  std::cout << "SimpleRPC duration: " << elapsed.count() << " ms\n";

  return grpc::Status::OK;
}

grpc::Status ServiceImpl::ServerStreaming(grpc::ServerContext* context, const grpc::ServerReader<routeguide::Point>* reader, routeguide::RouteSummary* summary) override {
  auto start = std::chrono::high_resolution_clock::now();

  routeguide::Point point;
  while (reader->Read(&point)) {
    // Simulate some processing
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> elapsed = end - start;
  std::cout << "ServerStreaming duration: " << elapsed.count() << " ms\n";

  return grpc::Status::OK;
}

grpc::Status ServiceImpl::ClientStreaming(grpc::ServerContext* context, grpc::ServerReaderWriter<routeguide::Feature, routeguide::Point>* stream) override {
  auto start = std::chrono::high_resolution_clock::now();

  routeguide::Point point;
  while (stream->Read(&point)) {
    // Simulate some processing
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> elapsed = end - start;
  std::cout << "ClientStreaming duration: " << elapsed.count() << " ms\n";

  return grpc::Status::OK;
}

grpc::Status ServerImpl::BidirectionalStream(grpc::ServerContext* context, grpc::ServerReaderWriter<routeguide::Feature, routeguide::Point>* stream) override {
  auto start = std::chrono::high_resolution_clock::now();

  routeguide::Point point;
  while (stream->Read(&point)) {
    // Simulate some processing
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> elapsed = end - start;
  std::cout << "BidirectionalStream duration: " << elapsed.count() << " ms\n";

  return grpc::Status::OK;
}
*/
