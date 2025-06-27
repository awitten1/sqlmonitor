
#include "duckdb.hpp"
#include <iostream>

#include <gflags/gflags.h>
#include <fmt/core.h>

DEFINE_string(data_dir, "data_files/",
                "directory to store files");
DEFINE_int32(interval_seconds, 10,
                "frequency to read data");

int main(int argc, char** argv) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    duckdb::DuckDB db;
    duckdb::Connection conn(db);
    auto result = conn.Query("select 1");
    assert(!result->HasError());
    result = conn.Query(fmt::format(R"(
        INSTALL ducklake;
        INSTALL sqlite;

        ATTACH 'ducklake:sqlite:metadata.sqlite' AS metrics
            (DATA_PATH '{}');
        USE metrics;
        )", FLAGS_data_dir));
    return 0;
}