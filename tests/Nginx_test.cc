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

TEST(GrpcIntegrationTest, BasicConnectivityTest) {
    // Constructing the grpcurl command to check basic connectivity
    std::string cmd = "echo '{\"image_data\":\"SGVsbG8=\"}' | grpcurl -plaintext -proto ../proto/services.proto -d @ localhost:80 imageTransfer.ImageService/SendReceiveImageStream";
    std::string output = exec(cmd.c_str());

    // Example of checking that the output contains a certain expected value
    // Adjust the expected response text according to what the server should return for a basic connectivity check
    EXPECT_TRUE(output.find("expected response") != std::string::npos);
}
