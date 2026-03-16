#pragma once

#include "threadsafe_queue.h"

#include <atomic>
#include <cstddef>
#include <future>
#include <functional>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

struct SchedulerStats {
    std::size_t submitted = 0;
    std::size_t completed = 0;
    std::size_t active_workers = 0;
    std::size_t queued = 0;
};

class TaskScheduler {
public:
    explicit TaskScheduler(std::size_t worker_count = std::thread::hardware_concurrency());
    ~TaskScheduler();

    TaskScheduler(const TaskScheduler&) = delete;
    TaskScheduler& operator=(const TaskScheduler&) = delete;

    template <typename Func, typename... Args>
    auto submit(Func&& func, Args&&... args)
        -> std::future<std::invoke_result_t<Func, Args...>>;

    void wait_for_all();
    void shutdown();
    SchedulerStats stats() const;

private:
    void worker_loop();

    ThreadSafeQueue<std::function<void()>> work_queue_;
    std::vector<std::thread> workers_;

    std::atomic<bool> accepting_{true};
    std::atomic<std::size_t> submitted_{0};
    std::atomic<std::size_t> completed_{0};
    std::atomic<std::size_t> active_workers_{0};

    mutable std::mutex completion_mutex_;
    std::condition_variable completion_cv_;
};

template <typename Func, typename... Args>
auto TaskScheduler::submit(Func&& func, Args&&... args)
    -> std::future<std::invoke_result_t<Func, Args...>> {
    using ResultType = std::invoke_result_t<Func, Args...>;

    if (!accepting_.load()) {
        throw std::runtime_error("scheduler is shutting down; no new tasks accepted");
    }

    auto task = std::make_shared<std::packaged_task<ResultType()>>(
        std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
    );

    std::future<ResultType> future = task->get_future();
    work_queue_.push([task]() mutable {
        (*task)();
    });
    submitted_.fetch_add(1);

    return future;
}
