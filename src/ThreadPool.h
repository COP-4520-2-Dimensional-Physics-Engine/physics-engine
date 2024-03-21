#pragma once
#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
  ThreadPool(size_t numThreads);
  ~ThreadPool();

  template <typename F, typename ...Args>
  void enqueue(F&& f, Args&&...args);

private:
  std::vector<std::thread> threads;
  std::queue<std::function<void()>> tasks;

  std::mutex queueMutex;
  std::condition_variable condition;
  bool stop = false;
};
