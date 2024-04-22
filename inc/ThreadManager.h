#pragma once

#include <iostream>
#include <thread>
#include <functional>

/*
  * Each Thread should have a runtime, cleanup, signal reader that will pass in information with a name, and a slot to pass in information
  */
struct ThreadArgument {
  const char* threadname;
  std::function<bool(void)> Runtime;
  std::function<void(void)> Cleanup;
};

template<size_t ThreadsAmount>
class ThreadManager {
public:
  // Special Member Functions
  constexpr ThreadManager(ThreadArgument* args);
  ~ThreadManager();
  ThreadManager(const ThreadManager&) = delete;
  ThreadManager& operator=(const ThreadManager&) = delete;
  ThreadManager(ThreadManager&&) = delete;
  ThreadManager& operator=(ThreadManager&&) = delete;
private:
  std::thread threads[ThreadsAmount];
};



template<size_t ThreadsAmount>
constexpr ThreadManager<ThreadsAmount>::ThreadManager(ThreadArgument* args) {
  for (size_t i = 0; i < ThreadsAmount; ++i) {
    threads[i] = std::thread(this, args[i].Runtime);
  }
}

template<size_t ThreadsAmount>
ThreadManager<ThreadsAmount>::~ThreadManager() {
  for (auto& thread : threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
} 
