#include "ddl.hpp"
#include <cassert>

void DDL(duckdb::Connection &conn) {
    auto res = conn.Query(R"(
        CREATE TABLE IF NOT EXISTS process_metrics (
            pid BIGINT,
            ts TIMESTAMP,
            interval INTERVAL,
            cpu_seconds BIGINT,
            major_faults INT,
            minor_faults INT,
        )
        )");
    assert(!res->HasError());

    res = conn.Query(R"(
        CREATE TABLE IF NOT EXISTS processes (
            pid BIGINT,
            start_time TIMESTAMP,
            cmdline VARCHAR,
            comm VARCHAR,
            PRIMARY KEY (pid, start_time)
        );
        )");

    assert(!res->HasError());
}