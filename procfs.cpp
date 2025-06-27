
#include <chrono>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <mutex>
#include <optional>
#include <ratio>
#include <string>
#include <unistd.h>
#include "procfs.hpp"

using namespace std::filesystem;

static bool is_int(const std::string& s) {
    try {
        std::stoi(s);
        return true;
    } catch(...) {
        return false;
    }
}


static std::mutex m;
static std::chrono::system_clock::time_point system_time_start() {
    static std::optional<std::chrono::system_clock::time_point> start_time;

    {
        std::lock_guard<std::mutex> lg(m);
        if (start_time.has_value()) {
            return *start_time;
        }
    }
    std::ifstream i("/proc/uptime");
    double seconds;
    i >> seconds;

    auto s = std::chrono::system_clock::now() - double_precision_seconds(seconds);
    {
        std::lock_guard<std::mutex> lg(m);
        start_time = std::chrono::time_point_cast<std::chrono::system_clock::duration>(s);
        return *start_time;
    }
}

const long page_size = sysconf(_SC_PAGESIZE);
const long ticks_per_second = sysconf(_SC_CLK_TCK);

void forEachProcess(std::function<void(PidInfo)> f) {
    path proc{"/proc"};

    for (auto const& dir_entry : directory_iterator{proc}) {
        std::string pids = dir_entry.path().filename().string();
        if (!is_int(pids)) {
            continue;
        }
        int pid = std::stoi(pids);
        PidInfo s(pid);
        f(s);
    }
}

void ProcPidStat::ReadProcFile() {
    std::ifstream f(fmt::format("/proc/{}/stat", pid), std::ios::in);
    std::string unused;

    for (int entry = 1; entry < 52; ++entry) {
        if (entry == 1) {
            f >> pid;
        }
        else if (entry == 2) {
            std::getline(f, unused, '(');
            std::getline(f, comm, ')');
        }
        else if (entry == 3) {
            f >> state;
        }
        else if (entry == 4) {
            f >> ppid;
        }
        else if (entry == 10) {
            f >> minflt;
        }
        else if (entry == 12) {
            f >> majflt;
        }
        else if (entry == 14) {
            long unsigned utime_ticks;
            f >> utime_ticks;
            utime = double_precision_seconds(static_cast<double>(utime_ticks) / ticks_per_second);
        }
        else if (entry == 15) {
            long unsigned stime_ticks;
            f >> stime_ticks;
            stime = double_precision_seconds(static_cast<double>(stime_ticks) / ticks_per_second);
        }
        else if (entry == 23) {
            f >> vsize_bytes;
        }
        else if (entry == 24) {
            f >> rss_bytes;
            rss_bytes *= page_size;
        }
        else if (entry == 22) {
            long unsigned starttime;
            f >> starttime;
            start_time = system_time_start() + std::chrono::seconds(starttime / ticks_per_second);
        } else {
            f >> unused;
        }
    }
}