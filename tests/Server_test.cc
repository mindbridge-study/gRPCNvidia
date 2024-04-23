#include <gtest/gtest.h>
#include <grpcpp/grpcpp.h>
#include "services.grpc.pb.h"  // Include the generated gRPC header

class ImageServiceTest : public ::testing::Test {
protected:
    std::unique_ptr<imageTransfer::ImageService::Stub> stub_;
    grpc::ClientContext context;
    std::shared_ptr<grpc::Channel> channel;

    ImageServiceTest() {
        // Create a channel and stub to communicate with our gRPC service
        channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
        stub_ = imageTransfer::ImageService::NewStub(channel);
    }

    virtual ~ImageServiceTest() {
    }
};

TEST_F(ImageServiceTest, SendReceiveImageStream) {
    // Create a stream for sending requests and receiving responses
    auto stream = stub_->SendReceiveImageStream(&context);

    // Send an image request
    imageTransfer::ImageRequest request;
    request.set_image_data(/* load your PNG data */);
    stream->Write(request);

    // Receive a response
    imageTransfer::ImageResponse response;
    stream->Read(&response);

    // Check the received response
    EXPECT_EQ(response.message(), "expected response");

    // Close the stream
    stream->WritesDone();
    grpc::Status status = stream->Finish();

    // Check the status of the operation
    EXPECT_TRUE(status.ok());
}
