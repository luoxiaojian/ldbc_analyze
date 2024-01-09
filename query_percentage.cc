#include "utils.h"

#include <assert.h>

#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <filesystem>

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <log_path_prefix> <log_num> <interval(min)>" << std::endl;
        return 1;
    }

    std::string prefix = argv[1];
    int log_num = std::stoi(argv[2]);
    int interval_min = std::stoi(argv[3]);

    int64_t begin_time = std::numeric_limits<int64_t>::max();
    int64_t end_time = 0;
    std::vector<std::vector<Record>> records_list;
    std::vector<size_t> warmup_num;
    for (int i = 0; i < log_num; ++i) {
        std::string log_path = prefix + std::to_string(i);
        if (!std::filesystem::exists(log_path)) {
            std::cerr << "Log file " << log_path << " does not exist." << std::endl;
            continue;
        }
        std::vector<Record> records = parse_log(log_path);
        size_t warmup_idx = warmup_count(records);
        warmup_num.push_back(warmup_idx);
        if (records[warmup_idx].start < begin_time) {
            begin_time = records[warmup_idx].start;
        }
        if (records.back().end > end_time) {
            end_time = records.back().end;
        }
        records_list.push_back(std::move(records));
    }

    std::map<int, std::vector<int>> cpu_time;
    int64_t interval_microsec = interval_min;
    interval_microsec *= 60 * 1000000;
    size_t interval_num = (end_time - begin_time + interval_microsec - 1) / interval_microsec;

    log_num = records_list.size();
    for (int i = 0; i < log_num; ++i) {
        for (size_t j = warmup_num[i]; j < records_list[i].size(); ++j) {
            auto start_time = records_list[i][j].start;
            auto end_time = records_list[i][j].end;
            auto type = records_list[i][j].type;
            auto iter = cpu_time.find(type);
            if (iter == cpu_time.end()) {
                cpu_time[type] = std::vector<int>(interval_num, 0);
                iter = cpu_time.find(type);
            }
            size_t start_interval_idx = (start_time - begin_time) / interval_microsec;
            size_t end_interval_idx = (end_time - begin_time) / interval_microsec;
            if (start_interval_idx == end_interval_idx) {
                iter->second[start_interval_idx] += (end_time - start_time);
            } else {
                assert(end_interval_idx = start_interval_idx + 1);
                iter->second[start_interval_idx] += (((start_interval_idx + 1) * interval_microsec) + begin_time - start_time);
                iter->second[end_interval_idx] += (records_list[i][j].end - ((end_interval_idx * interval_microsec) + begin_time));
            }
        }
    }

    std::vector<int64_t> interval_total_cpu_time(interval_num, 0);
    for (auto& pair : cpu_time) {
        for (size_t k = 0; k != interval_num; ++k) {
            interval_total_cpu_time[k] += pair.second[k];
        }
    }

    std::filesystem::create_directory("./percentage");
    for (auto& pair : cpu_time) {
        {
            std::string output_path = prefix + "./percentage/i" + std::to_string(interval_min) + "_q" + std::to_string(pair.first);
            std::ofstream fout(output_path);
            for (size_t i = 0; i < pair.second.size(); ++i) {
                fout << static_cast<double>(pair.second[i]) / static_cast<double>(interval_total_cpu_time[i]) << std::endl;
            }
            fout.flush();
        }
    }

    return 0;
}
