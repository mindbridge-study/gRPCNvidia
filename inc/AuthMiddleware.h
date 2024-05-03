#pragma once

#include <grpcpp/grpcpp.h>
#include <grpcpp/support/status.h>
#include <jwt-cpp/jwt.h>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <map>
#include <chrono>

using grpc::experimental::InterceptionHookPoints;
using grpc::experimental::Interceptor;
using grpc::experimental::InterceptorBatchMethods;
using grpc::experimental::ServerInterceptorFactoryInterface;
using grpc::experimental::ServerRpcInfo;
using grpc::Status;
using grpc::StatusCode;

// Project ID
const std::string ProjectID = "gemini-122e9";

class FirebaseInterceptor : public grpc::experimental::Interceptor {
private:
  // Global cache for public keys and the last fetch time
  std::map<std::string, std::string> key_cache;
  std::chrono::system_clock::time_point last_fetch_time;
public:
  void Intercept(grpc::experimental::InterceptorBatchMethods* methods) override; // Function that Intercepts the RPC Request
private:
  bool fetch_keys(); // Fetch Public Keys from Firebase
  bool verifyToken(const std::string& token); // JWT Vertification
};

/*
* Factory for creating the Interceptor
*/
class LoggingInterceptorFactory : public ServerInterceptorFactoryInterface {
public:
  Interceptor* CreateServerInterceptor(ServerRpcInfo* info) override {
    return new FirebaseInterceptor();
  }
};
