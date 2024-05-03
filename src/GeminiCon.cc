#include "GeminiCon.h"

std::pair<std::string, bool> GeminiCon::SendRequests(const std::vector<uint8_t>& imageBytes) {
  // Base64-encode the image bytes
  std::string encodedImage = base64::to_base64(std::string(imageBytes.begin(), imageBytes.end()));

  // JSON body for the POST request
  nlohmann::json data = {
    {"prompt", prompt},
    {"image", encodedImage}
  };

  // Headers for the POST request
  cpr::Header headers = {
    {"Authorization", "Bearer " + apikey},
    {"Content-Type", "application/json"}
  };

  // Send the POST request
  cpr::Response r = cpr::Post(cpr::Url{"https://api.gemini-models.io/generate"},
                              headers,
                              cpr::Body{data.dump()},
                              cpr::Timeout{10000});  // Timeout in milliseconds

  // Check the response status and return the results
  if (r.status_code == 200) {
    return std::make_pair(r.text, true);  // Return the raw JSON response text
  } else {
    // Log error and return a JSON-formatted error message
    std::cerr << "Failed to get a valid response: " << r.status_code << " " << r.error.message << std::endl;
    nlohmann::json error_response = {
      {"error", true},
      {"status_code", r.status_code},
      {"message", "Failed to get a valid response"},
      {"error_details", r.error.message}
    };
    return std::make_pair(error_response.dump(4), false);
  }
}
