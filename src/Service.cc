#include "Service.h"

ServiceImpl::~ServiceImpl() {
  _queue->Shutdown();
  _server->Shutdown();
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


// UploadImage - A
grpc::Status ServiceImpl::UploadImage(grpc::ServerContext* context, grpc::ServerReader<routeguide::ImageChunk>* reader, routeguide::UploadStatus* status) {
  std::vector<uint8_t> imageData;
  routeguide::ImageChunk chunk;
  while (reader->Read(&chunk)) {
    std::vector<uint8_t> data(chunk.data().begin(), chunk.data().end());
    imageData.insert(imageData.end(), data.begin(), data.end());
  }

  // Process image data here

  status->set_success(true);
  status->set_message("Image received successfully");
  return grpc::Status::OK;
}

grpc::Status ServiceImpl::BidirectionalImageTransfer(grpc::ServerContext* context, grpc::ServerReaderWriter<routeguide::ImageChunk, routeguide::ImageChunk>* stream) {
  routeguide::ImageChunk chunk;
  while (stream->Read(&chunk)) {
    // Echo back the chunk received
    stream->Write(chunk);
  }
  return grpc::Status::OK;
}

void ServiceImpl::Run() {
  std::string server_address("0.0.0.0:50051");

  // Build server
  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(this);
  _queue = builder.AddCompletionQueue();
  _server = builder.BuildAndStart();

  // Run server
  std::cout << "Server listening on " << server_address << std::endl;
  HandleRPCs();
}
