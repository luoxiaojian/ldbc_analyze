#include "utils.h"

#include <iostream>
#include <map>
#include <vector>
#include <fstream>

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

    std::map<int, std::vector<int>> frequencies;
    std::map<int, std::vector<int64_t>> latencies;
    int64_t interval_microsec = interval_min;
    interval_microsec *= 60 * 1000000;
    size_t interval_num = (end_time - begin_time + interval_microsec - 1) / interval_microsec;

    log_num = records_list.size();
    for (int i = 0; i < log_num; ++i) {
        for (size_t j = warmup_num[i]; j < records_list[i].size(); ++j) {
            size_t interval_idx = (records_list[i][j].start - begin_time) / interval_microsec;
            {
                auto iter = frequencies.find(records_list[i][j].type);
                if (iter == frequencies.end()) {
                    frequencies[records_list[i][j].type] = std::vector<int>(interval_num, 0);
                    iter = frequencies.find(records_list[i][j].type);
                }
                ++iter->second[interval_idx];
            }
            {
                auto iter = latencies.find(records_list[i][j].type);
                if (iter == latencies.end()) {
                    latencies[records_list[i][j].type] = std::vector<int64_t>(interval_num, 0);
                    iter = latencies.find(records_list[i][j].type);
                }
                iter->second[interval_idx] += (records_list[i][j].end - records_list[i][j].start);
            }
        }
    }

    for (auto& pair : frequencies) {
        {
            std::string output_path = prefix + ".freq_" + std::to_string(interval_min) + "_q" + std::to_string(pair.first);
            std::ofstream fout(output_path);
            for (size_t i = 0; i < pair.second.size(); ++i) {
                fout << pair.second[i] << std::endl;
            }
            fout.flush();
        }
        {
            std::string output_path = prefix + ".lat_" + std::to_string(interval_min) + "_q" + std::to_string(pair.first);
            std::ofstream fout(output_path);
            auto& freq = pair.second;
            auto& lat = latencies[pair.first];

            size_t interval_num = freq.size();
            for (size_t i = 0; i < interval_num; ++i) {
                if (freq[i] == 0) {
                    fout << 0 << std::endl;
                } else {
                    fout << static_cast<double>(lat[i]) / static_cast<double>(freq[i]) << std::endl;
                }
            }

        }
    }

    return 0;
}