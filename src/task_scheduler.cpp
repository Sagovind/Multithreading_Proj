#include "task_scheduler.h"

#include <stdexcept>

TaskScheduler::TaskScheduler(std::size_t worker_count) {
    if (worker_count == 0) {
        worker_count = 1;
    }

    workers_.reserve(worker_count);
    for (std::size_t i = 0; i < worker_count; ++i) {
        workers_.emplace_back(&TaskScheduler::worker_loop, this);
    }
}

TaskScheduler::~TaskScheduler() {
    shutdown();
}

void TaskScheduler::worker_loop() {
    std::function<void()> task;
    while (work_queue_.wait_pop(task)) {
        active_workers_.fetch_add(1);
        try {
            task();
        } catch (...) {
            // Exceptions are captured by packaged_task and rethrown on future.get().
        }
        active_workers_.fetch_sub(1);
        completed_.fetch_add(1);
        completion_cv_.notify_all();
    }
}

void TaskScheduler::wait_for_all() {
    std::unique_lock<std::mutex> lock(completion_mutex_);
    completion_cv_.wait(lock, [this] {
        return completed_.load() == submitted_.load() &&
               active_workers_.load() == 0 &&
               work_queue_.empty();
    });
}

void TaskScheduler::shutdown() {
    bool expected = true;
    if (!accepting_.compare_exchange_strong(expected, false)) {
        return;
    }

    work_queue_.close();
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    completion_cv_.notify_all();
}

SchedulerStats TaskScheduler::stats() const {
    SchedulerStats s;
    s.submitted = submitted_.load();
    s.completed = completed_.load();
    s.active_workers = active_workers_.load();
    s.queued = work_queue_.size();
    return s;
}
