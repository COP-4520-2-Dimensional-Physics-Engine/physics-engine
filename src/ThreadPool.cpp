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

template <typename F, typename ...Args>
void ThreadPool::enqueue(F&& f, Args&&...args) {
  auto task = std::make_shared<std::function<void()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
  {
    std::unique_lock<std::mutex> lock(queueMutex);
    tasks.push([task] { (*task)(); });
  }
  condition.notify_one();
}
