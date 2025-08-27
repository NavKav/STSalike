//
// Created by navid on 22/08/2025.
//

#include "ThreadPool.h"

ThreadPool& ThreadPool::getInstance() {
    static ThreadPool instance;
    return instance;
}

ThreadPool::ThreadPool() : stop(false) {
    size_t numThreads = std::thread::hardware_concurrency();

    if (numThreads == 0) {
        numThreads = 4;
    }

    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] {
            this->workerLoop();
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();

    for (std::thread& worker : workers) {
        worker.join();
    }
}

void ThreadPool::workerLoop() {
    while (true) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] {
                return this->stop || !this->tasks.empty();
            });

            if (this->stop && this->tasks.empty()) {
                return;
            }

            task = std::move(this->tasks.front());
            this->tasks.pop();
        }
        task();
    }
}

void ThreadPool::enqueue(Task task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        if (stop) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        tasks.emplace(std::move(task));
    }
    condition.notify_one();
}
