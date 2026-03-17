FROM ubuntu:24.04 AS build
RUN apt-get update && apt-get install -y --no-install-recommends build-essential cmake ninja-build ca-certificates && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY . .
RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTS=ON
RUN cmake --build build --target scheduler_demo task_scheduler_tests
RUN ctest --test-dir build --output-on-failure

FROM ubuntu:24.04 AS runtime
RUN useradd -m appuser
WORKDIR /app
COPY --from=build /app/build/scheduler_demo /app/scheduler_demo
USER appuser
ENTRYPOINT ["/app/scheduler_demo"]