#include "utils.h"

#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <log_path> <interval(min)>" << std::endl;
        return 1;
    }

    std::string log_path = argv[1];
    int interval_min = std::stoi(argv[2]);
    int64_t interval_microsec = interval_min;
    interval_microsec *= 60 * 1000000;

    std::vector<Record> records = parse_log(log_path);
    size_t warmup_idx = warmup_count(records);
    std::cout << "Warmup count: " << warmup_idx << std::endl;
    std::cout << "Warmup time: " << get_seconds(records[warmup_idx].start) << " s" << std::endl;
    std::cout << "Total time: " << get_seconds(records.back().end) << " s" << std::endl;

    std::vector<int64_t> idle_time;

    int64_t interval_begin = records[warmup_idx].start;
    int64_t interval_end = interval_begin + interval_microsec;
    int64_t interval_busy = 0;
    for (size_t i = warmup_idx; i < records.size(); ++i) {
        if (records[i].start >= interval_end) {
            idle_time.push_back(interval_microsec - interval_busy);

            interval_begin = interval_end;
            interval_end = interval_begin + interval_microsec;
            assert(interval_end > records[i].start);
            assert(interval_end > records[i].end);
            interval_busy = records[i].end - records[i].start;
        } else {
            if (records[i].end >= interval_end) {
                interval_busy += interval_end - records[i].start;
                idle_time.push_back(interval_microsec - interval_busy);

                interval_begin = interval_end;
                interval_end = interval_begin + interval_microsec;
                assert(interval_end > records[i].end);
                interval_busy = records[i].end - interval_begin;
            } else {
                interval_busy += records[i].end - records[i].start;
            }
        }
    }
    if (interval_busy != 0) {
        idle_time.push_back((records.back().end - interval_begin) - interval_busy);
    }

    std::string output_path = log_path + ".idle_" + std::to_string(interval_min);
    std::ofstream fout(output_path);
    for (size_t i = 0; i < idle_time.size(); ++i) {
        fout << idle_time[i] << std::endl;
    }
    fout.flush();

    return 0;
}