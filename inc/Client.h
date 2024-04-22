#pragma once
/*
* There are two parts to implementing a gRPC Client:
* 1. Implementing the Service Interface Generated from the .proto file
* 2. Running a Client to send requests to the Server, and receive a response
*/

#include <iostream>
#include "TransferData.h"
#include <grpcpp/grpcpp.h>
#include "services.grpc.pb.h"

template<typename... Params>
class Client final {
public: 
  constexpr Client(Params... params) : data(params...), stub_(imageTransfer::ImageService::NewStub(channel_)) {}
  ~Client() = default;
  Client(const Client&) = delete;
  Client& operator=(const Client&) = delete;
  Client(Client&&) = delete;
  Client& operator=(Client&&) = delete;

  void SendImage(const std::string& image_data);
  void ReceiveImageStream();

private:
  grpc::ClientContext context;
  std::unique_ptr<grpc::ClientReaderWriter<imageTransfer::ImageRequest, imageTransfer::ImageResponse>> stream;
  std::shared_ptr<grpc::Channel> channel_;
  std::unique_ptr<imageTransfer::ImageService::Stub> stub_;
  Data<Params...> data;
};

template<typename... Params>
void Client<Params...>::SendImage(const std::string& image_data) {
    // Prepare the request with image data
    imageTransfer::ImageRequest request;
    request.set_image_data(image_data);

    // Begin communication with server
    stream = stub_->SendReceiveImageStream(&context);

    // Write the request to the stream
    stream->Write(request);
    stream->WritesDone();
}

template<typename... Params>
void Client<Params...>::ReceiveImageStream() {
    imageTransfer::ImageResponse response;
    while (stream->Read(&response)) {
        std::cout << "Received message: " << response.message() << std::endl;
    }
    grpc::Status status = stream->Finish();
    if (!status.ok()) {
        std::cerr << "Stream failed: " << status.error_message() << std::endl;
    }
}
