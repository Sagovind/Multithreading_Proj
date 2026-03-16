# Multithreaded Job Processing System (Mini Task Scheduler)

A simple C++ project that demonstrates core multithreading concepts through a producer-consumer style task scheduler.

## Concepts Demonstrated

- `std::mutex` and `std::lock_guard`: protect shared queue state.
- `std::condition_variable`: worker threads block until work is available.
- `std::atomic`: lock-free counters for submitted/completed jobs and active workers.
- `std::future` and `std::packaged_task`: collect task results and propagate exceptions.
- `std::async`: run an independent aggregation task in parallel in the demo app.

## Project Structure

```text
.
|-- .github/workflows/ci.yml
|-- include/
|   |-- task_scheduler.h
|   `-- threadsafe_queue.h
|-- src/
|   |-- main.cpp
|   `-- task_scheduler.cpp
|-- tests/
|   `-- task_scheduler_tests.cpp
`-- CMakeLists.txt
```

## Build and Run

### Prerequisites

- CMake 3.16+
- C++ compiler with C++20 support

### Build

```bash
cmake -S . -B build -DENABLE_TESTS=ON
cmake --build build
```

### Run Demo

```bash
./build/scheduler_demo
```

On Windows with multi-config generators, use:

```bash
cmake --build build --config Release
.\build\Release\scheduler_demo.exe
```

## Run Unit Tests

```bash
ctest --test-dir build --output-on-failure
```

On Windows multi-config generators:

```bash
ctest --test-dir build -C Release --output-on-failure
```

## Test Coverage Scope

- Executes all submitted tasks exactly once.
- Handles concurrent producers safely.
- Properly propagates exceptions through `future.get()`.
- Rejects new submissions after shutdown.

## GitHub Actions

The workflow at `.github/workflows/ci.yml` builds and tests on:

- Ubuntu latest
- Windows latest

It runs automatically on every push and pull request.

## Suggested Future Improvements

- Priority queue support (high/normal/low jobs).
- Task cancellation tokens.
- Timed/scheduled jobs.
- Benchmark mode for throughput and latency.
