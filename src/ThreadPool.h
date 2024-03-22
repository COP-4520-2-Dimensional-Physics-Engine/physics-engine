#pragma once
#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <atomic>
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
      queuedTasks += 1;
      tasks.push([task, this] {
        (*task)();
        {
          std::unique_lock<std::mutex> lock(queueMutex);
          queuedTasks -= 1;
          waitCondition.notify_one();
        }
      });
    }
    condition.notify_one();
  }

  void wait();

private:
  std::vector<std::thread> threads;
  std::queue<std::function<void()>> tasks;
  std::atomic<int> queuedTasks = 0;

  std::mutex queueMutex;
  std::condition_variable condition;
  std::condition_variable waitCondition;
  bool stop = false;
};
