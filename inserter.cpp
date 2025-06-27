
#include "inserter.hpp"
#include <chrono>
#include <thread>
#include <filesystem>

void RunInsertions(duckdb::Connection &conn, std::chrono::seconds s) {
    for (;;) {
        std::this_thread::sleep_for(s);
    }
}