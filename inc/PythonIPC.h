#pragma once

#include <zmqpp/zmqpp.hpp>
#include <string>
#include <iostream>
#include <thread>

constexpr char endpoint[] = "tcp://localhost:5555";


class PythonIPC {
public:
  explicit PythonIPC();
  ~PythonIPC();

  std::string sendAndReceive(const std::string& message);

  // Deleted constructors and operators for copy and move
  PythonIPC(const PythonIPC&) = delete;
  PythonIPC& operator=(const PythonIPC&) = delete;
  PythonIPC(PythonIPC&&) = delete;
  PythonIPC& operator=(PythonIPC&&) = delete;

private:
  zmqpp::context context;
  zmqpp::socket socket;
};

