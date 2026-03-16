cmake_minimum_required(VERSION 3.16)

project(Multithreading_Proj VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(task_scheduler
    src/task_scheduler.cpp
)

target_include_directories(task_scheduler
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(scheduler_demo
    src/main.cpp
)

target_link_libraries(scheduler_demo
    PRIVATE task_scheduler
)

option(ENABLE_TESTS "Build unit tests" ON)

if(ENABLE_TESTS)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(task_scheduler_tests
        tests/task_scheduler_tests.cpp
    )

    target_link_libraries(task_scheduler_tests
        PRIVATE task_scheduler GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(task_scheduler_tests)
endif()
