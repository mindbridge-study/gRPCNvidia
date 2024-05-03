#include "AuthMiddleware.h"

void FirebaseInterceptor::Intercept(grpc::experimental::InterceptorBatchMethods* methods) {
  if (methods->QueryInterceptionHookPoint(InterceptionHookPoints::PRE_SEND_INITIAL_METADATA)) {
    auto metadata = methods->GetRecvInitialMetadata();
    auto auth_header = metadata->find("authorization");
    if (auth_header != metadata->end()) {
      std::string token = std::string(auth_header->second.data(), auth_header->second.length());
      if (token.substr(0, 7) == "Bearer ") {
        token = token.substr(7); // Remove "Bearer " prefix
        if (!verifyToken(token)) { // Token is invalid
          std::cerr << "Invalid token provided!" << std::endl;
          auto status = Status(StatusCode::UNAUTHENTICATED, "Token is not valid");
          methods->ModifySendStatus(status);
          return;
        }
      } else { // Malformed Token
        std::cerr << "Malformed authorization header. Expected 'Bearer [token]'" << std::endl;
        auto status = Status(StatusCode::UNAUTHENTICATED, "Malformed authorization token");
        methods->ModifySendStatus(status);
        return;
      }
    } else { // No Token Provided
      std::cerr << "No auth token provided!" << std::endl;
      auto status = Status(StatusCode::UNAUTHENTICATED, "No auth token provided!");
      methods->ModifySendStatus(status);
      return;
    }
  }
  methods->Proceed();
}

bool FirebaseInterceptor::fetch_keys() {
  // Check if last fetch was less than 6 hours ago
  auto now = std::chrono::system_clock::now();
  if (!key_cache.empty() && std::chrono::duration_cast<std::chrono::hours>(now - last_fetch_time).count() < 6) {
      return true;  // Use cached keys
  }

  std::string public_keys_url = "https://www.googleapis.com/robot/v1/metadata/x509/securetoken@system.gserviceaccount.com";
  cpr::Response r = cpr::Get(cpr::Url{public_keys_url}, cpr::Header{{"Content-Type", "application/json"}});
  if (r.status_code != 200) {
      std::cerr << "Failed to fetch public keys: " << r.error.message << std::endl;
      return false;
  }

  // Clear existing keys and reset cache
  key_cache.clear();
  auto public_keys = nlohmann::json::parse(r.text);
  for (auto& key : public_keys.items()) {
      key_cache[key.key()] = key.value().get<std::string>();
  }
  last_fetch_time = now;
  return true;
}

bool FirebaseInterceptor::verifyToken(const std::string& token) {
  if (!fetch_keys()) {
    std::cerr << "Could not fetch keys for verification." << std::endl;
    return false;
  }

  try {
    auto decoded = jwt::decode(token);
    std::string kid = decoded.get_header_claim("kid").as_string();
    if (key_cache.count(kid) == 0) {
      throw std::runtime_error("Invalid 'kid' in JWT header");
    }

    auto verifier = jwt::verify()
      .allow_algorithm(jwt::algorithm::rs256(key_cache[kid], "", "", ""))
      .with_issuer("https://securetoken.google.com/" + ProjectID)
      .with_audience(ProjectID);

    verifier.verify(decoded);

    std::cout << "JWT is valid" << std::endl;
    return true;
  } catch (const std::exception& e) {
    return false;
    std::cerr << "JWT validation failed: " << e.what() << std::endl;
  }
}
