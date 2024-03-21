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
  void enqueue(F&& f, Args&&...args) {
    auto task = std::make_shared<std::function<void()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      tasks.push([task] { (*task)(); });
    }
    condition.notify_one();
  }

private:
  std::vector<std::thread> threads;
  std::queue<std::function<void()>> tasks;

  std::mutex queueMutex;
  std::condition_variable condition;
  bool stop = false;
};
