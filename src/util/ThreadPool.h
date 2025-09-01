#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <stdexcept>
#include <utility>

using Task = std::move_only_function<void()>;

class ThreadPool {
public:
    static ThreadPool& getInstance() {
        static ThreadPool instance;
        return instance;
    }

    void enqueue(Task&& task) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (stop) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }
            tasks.emplace(std::move(task));
        }
        condition.notify_one();
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

private:
    ThreadPool() : stop(false) {
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

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();

        for (std::thread& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    void workerLoop() {
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
            if (task) {
                task();
            }
        }
    }

    std::vector<std::thread> workers;
    std::queue<Task> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};

inline ThreadPool& ThreadPool = ThreadPool::getInstance();


#endif // THREADPOOL_H