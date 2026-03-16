#include "task_scheduler.h"

#include <atomic>
#include <chrono>
#include <future>
#include <stdexcept>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

TEST(TaskSchedulerTests, ExecutesAllSubmittedTasks) {
    TaskScheduler scheduler(4);
    std::vector<std::future<int>> futures;

    for (int i = 1; i <= 100; ++i) {
        futures.push_back(scheduler.submit([i] { return i; }));
    }

    scheduler.wait_for_all();

    int total = 0;
    for (auto& f : futures) {
        total += f.get();
    }

    EXPECT_EQ(total, 5050);
    SchedulerStats stats = scheduler.stats();
    EXPECT_EQ(stats.submitted, 100U);
    EXPECT_EQ(stats.completed, 100U);
    EXPECT_EQ(stats.queued, 0U);
}

TEST(TaskSchedulerTests, FuturePropagatesExceptions) {
    TaskScheduler scheduler(2);

    auto bad_task = scheduler.submit([]() -> int {
        throw std::runtime_error("boom");
    });

    scheduler.wait_for_all();
    EXPECT_THROW((void)bad_task.get(), std::runtime_error);
}

TEST(TaskSchedulerTests, HandlesConcurrentProducers) {
    TaskScheduler scheduler(4);
    std::atomic<int> executed{0};

    constexpr int producer_count = 4;
    constexpr int jobs_per_producer = 50;

    std::vector<std::thread> producers;
    producers.reserve(producer_count);

    for (int p = 0; p < producer_count; ++p) {
        producers.emplace_back([&scheduler, &executed] {
            for (int i = 0; i < jobs_per_producer; ++i) {
                scheduler.submit([&executed] {
                    executed.fetch_add(1);
                });
            }
        });
    }

    for (auto& producer : producers) {
        producer.join();
    }

    scheduler.wait_for_all();

    EXPECT_EQ(executed.load(), producer_count * jobs_per_producer);
    SchedulerStats stats = scheduler.stats();
    EXPECT_EQ(stats.submitted, static_cast<std::size_t>(producer_count * jobs_per_producer));
    EXPECT_EQ(stats.completed, static_cast<std::size_t>(producer_count * jobs_per_producer));
}

TEST(TaskSchedulerTests, RejectsNewWorkAfterShutdown) {
    TaskScheduler scheduler(2);

    auto first = scheduler.submit([] {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        return 42;
    });

    scheduler.wait_for_all();
    scheduler.shutdown();

    EXPECT_EQ(first.get(), 42);
    EXPECT_THROW((void)scheduler.submit([] { return 7; }), std::runtime_error);
}
