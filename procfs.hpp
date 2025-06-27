#include <chrono>
#include <fstream>
#include <fmt/core.h>
#include <functional>

extern const long page_size;
extern const long ticks_per_second;

using double_precision_seconds = std::chrono::duration<double, std::ratio<1>>;

struct ProcPidStatus {
private:
    void ReadProcFile() {
        std::ifstream f(fmt::format("/proc/{}/status", pid), std::ios::in);

        std::string line;
        while (std::getline(f, line)) {
            auto delim = line.find(':');
            std::string key = line.substr(0, delim);
            std::string value_temp = line.substr(delim + 1);
            auto value_delim = value_temp.find_first_not_of(" \t");
            std::string value = value_temp.substr(value_delim);
            if (key == "voluntary_ctxt_switches") {
                voluntary_ctxt_switches = std::stoul(value);
            } else if (key == "nonvoluntary_ctxt_switches") {
                nonvoluntary_ctxt_switches = std::stoul(value);
            }
        }
    }
public:
    ProcPidStatus(int pid) : pid(pid) {
        ReadProcFile();
    }

    int pid;
    long unsigned voluntary_ctxt_switches;
    long unsigned nonvoluntary_ctxt_switches;
};

struct ProcPidStat {
private:
    void ReadProcFile();

public:

    ProcPidStat(int pid) : pid(pid) {
        ReadProcFile();
    }

    int pid;
    std::string comm;
    std::string state;
    int ppid;
    long unsigned minflt;
    long unsigned majflt;
    double_precision_seconds stime;
    double_precision_seconds utime;
    long unsigned vsize_bytes;
    long unsigned rss_bytes;
    std::chrono::time_point<std::chrono::system_clock> start_time;
};

class PidInfo {
public:
    PidInfo(int pid) : stat(pid), status(pid) {
    }
    ProcPidStat stat;
    ProcPidStatus status;
};


void forEachProcess(std::function<void(PidInfo)> f);