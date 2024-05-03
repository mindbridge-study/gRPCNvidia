#include <gtest/gtest.h>
#include "ServerImpl.h"

class ServerTest : public ::testing::Test {
protected:
    ServerImpl server;

    void SetUp() override {
        server.Run(8080);  // Example port, adjust as necessary
    }

    void TearDown() override {
        // Properly close server and cleanup any resources
        server.Shutdown();
    }

    // Helper function to simulate client requests
    grpc::Status SendPingRequest(const std::string& token) {
        grpc::ClientContext context;
        // Setting up the authorization token
        context.AddMetadata("authorization", "Bearer " + token);

        // Placeholder for ping request and response
        PingRequest request;
        PingResponse response;
        // Assume a client stub exists which can send a Ping request
        return clientStub->Ping(&context, request, &response);
    }

    grpc::Status SendImageUploadRequest(const std::string& token, const std::vector<uint8_t>& imageData) {
        grpc::ClientContext context;
        context.AddMetadata("authorization", "Bearer " + token);

        // Placeholder for image upload request and response
        ImageUploadRequest request;
        ImageUploadResponse response;
        // Simulate sending image data, assume a client stub method for this
        return clientStub->UploadImage(&context, imageData, &response);
    }
};

TEST_F(ServerTest, PingWithValidToken) {
    // Assuming `validToken` is a correctly signed JWT
    std::string validToken = "valid.jwt.token";
    auto status = SendPingRequest(validToken);
    ASSERT_TRUE(status.ok());
}

TEST_F(ServerTest, PingWithInvalidToken) {
    std::string invalidToken = "invalid.jwt.token";
    auto status = SendPingRequest(invalidToken);
    ASSERT_EQ(status.error_code(), grpc::UNAUTHENTICATED);
}

TEST_F(ServerTest, PingWithNoToken) {
    std::string noToken = "";
    auto status = SendPingRequest(noToken);
    ASSERT_EQ(status.error_code(), grpc::UNAUTHENTICATED);
}

TEST_F(ServerTest, ImageUploadWithValidToken) {
    std::string validToken = "valid.jwt.token";
    std::vector<uint8_t> imageData = { /* image data bytes */ };
    auto status = SendImageUploadRequest(validToken, imageData);
    ASSERT_TRUE(status.ok());
}

