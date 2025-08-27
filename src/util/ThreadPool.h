//
// Created by navid on 22/08/2025.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <stdexcept>

using Task = std::move_only_function<void()>;

class ThreadPool {
public:
    static ThreadPool& getInstance();

    void enqueue(Task task);

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

private:
    ThreadPool();
    ~ThreadPool();

    void workerLoop();

    std::vector<std::thread> workers;
    std::queue<std::move_only_function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};

#endif // THREADPOOL_H