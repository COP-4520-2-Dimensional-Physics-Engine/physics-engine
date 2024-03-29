#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t numThreads) {
  for (size_t i = 0; i < numThreads; i++) {
    threads.emplace_back([this] {
      while (true) {
        std::function<void()> task;
        {
          std::unique_lock<std::mutex> lock(queueMutex);
          condition.wait(lock, [this] { return stop || !tasks.empty(); });
          if (stop && tasks.empty()) {
            return;
          }
          task = std::move(tasks.front());
          tasks.pop();
        }
        task();
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(queueMutex);
    stop = true;
  }
  condition.notify_all();
  for (auto& thread : threads) {
    thread.join();
  }
}

void ThreadPool::wait() {
  std::unique_lock<std::mutex> lock(queueMutex);
  waitCondition.wait(lock, [this] { return queuedTasks == 0; });
}
