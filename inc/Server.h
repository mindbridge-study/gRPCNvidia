#pragma once
/*
 * There are two parts to implementing a gRPC Server:
 * 1. Implementing the Service Interface Generated from the .proto file
 * 2. Running a Server to actually listen to requests from clients, and return a response
 */

#include <iostream>
#include <grpcpp/grpcpp.h>
#include "services.grpc.pb.h"
#include "TransferData.h"

template<typename... Params>
class Server {
public:
    constexpr Server(Params... params) : data(params...), service_() {}
    ~Server() = default;
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&&) = delete;

    void Run();

private:
    class ImageServiceImpl final : public imageTransfer::ImageService::Service {
    public:
        grpc::Status SendReceiveImageStream(grpc::ServerContext* context,
                                            grpc::ServerReaderWriter<imageTransfer::ImageResponse, imageTransfer::ImageRequest>* stream) override {
            imageTransfer::ImageRequest request;
            while (stream->Read(&request)) {
                // Process each request
                std::string image_data = request.image_data();
                std::cout << "Received image data of size: " << image_data.size() << std::endl;

                // Send a response back to the client
                imageTransfer::ImageResponse response;
                response.set_message("Received your image, processing complete.");
                stream->Write(response);
            }
            return grpc::Status::OK;
        }
    };

    ImageServiceImpl service_;
    std::unique_ptr<grpc::Server> grpc_server_;
    Data<Params...> data;
};


template<typename... Params>
void Server<Params...>::Run() {
    std::string server_address("0.0.0.0:50051");
    grpc::ServerBuilder builder;

    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    builder.RegisterService(&service_);

    grpc_server_ = builder.BuildAndStart();
    std::cout << "Server listening on " << server_address << std::endl;

    grpc_server_->Wait();
}
