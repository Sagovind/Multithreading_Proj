#include "task_scheduler.h"

#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

int main() {
    TaskScheduler scheduler(4);
    std::vector<std::future<int>> results;

    std::cout << "Submitting jobs to the scheduler...\n";
    for (int i = 1; i <= 10; ++i) {
        results.push_back(scheduler.submit([i] {
            std::this_thread::sleep_for(std::chrono::milliseconds(40 * (i % 3 + 1)));
            return i * i;
        }));
    }

    // Demonstrates independent async work happening in parallel.
    auto async_total = std::async(std::launch::async, [&results] {
        int total = 0;
        for (auto& result : results) {
            total += result.get();
        }
        return total;
    });

    scheduler.wait_for_all();
    const int total = async_total.get();
    SchedulerStats summary = scheduler.stats();

    std::cout << "All jobs completed.\n";
    std::cout << "Sum of squares (1..10): " << total << "\n";
    std::cout << "Submitted: " << summary.submitted << "\n";
    std::cout << "Completed: " << summary.completed << "\n";
    std::cout << "Active workers: " << summary.active_workers << "\n";
    std::cout << "Queued jobs: " << summary.queued << "\n";

    scheduler.shutdown();
    return 0;
}
