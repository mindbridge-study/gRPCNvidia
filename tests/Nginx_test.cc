#include <gtest/gtest.h>
#include <cstdlib>
#include <array>
#include <memory>

// Function to execute a system command and return output
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

class GrpcServerTest : public ::testing::Test {
protected:
    static void SetUpTestCase() {
        // Start the server in a separate process or ensure it's running before tests start
        // system("./server &"); // Uncomment if you need to start the server programmatically
    }

    static void TearDownTestCase() {
        // Shutdown the server if it was started by the test
        // system("pkill server"); // Uncomment if the server needs to be killed
    }
};

TEST_F(GrpcServerTest, ImageUploadTest) {
    // Testing the UploadImage RPC method
    std::string cmd = "echo '{\"data\":\"SGVsbG8gd29ybGQh\"}' | grpcurl -plaintext -proto ../proto/routeguide.proto -d @ localhost:50051 routeguide.RouteGuide/UploadImage";
    std::string output = exec(cmd.c_str());

    // Check if the output indicates success
    EXPECT_TRUE(output.find("\"success\": true") != std::string::npos);
}

TEST_F(GrpcServerTest, BidirectionalImageTransferTest) {
    // Testing the BidirectionalImageTransfer RPC method
    std::string cmd = "echo '{\"data\":\"SGVsbG8gd29ybGQh\"}' | grpcurl -plaintext -proto ../proto/routeguide.proto -d @ localhost:50051 routeguide.RouteGuide/BidirectionalImageTransfer";
    std::string output = exec(cmd.c_str());

    // Check if the output contains expected data (assuming echo behavior)
    EXPECT_TRUE(output.find("\"data\": \"SGVsbG8gd29ybGQh\"") != std::string::npos);
}

