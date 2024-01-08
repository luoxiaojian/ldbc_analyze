#ifndef LDBC_ANALYZE_UTILS_H
#define LDBC_ANALYZE_UTILS_H

#include <stdlib.h>
#include <stdio.h>

#include <vector>
#include <string>
#include <filesystem>

struct Record {
    int type;
    int64_t start;
    int64_t end;
};

std::vector<Record> parse_log(const std::string& log_path) {
    FILE* fin = fopen(log_path.c_str(), "r");
    size_t file_size = std::filesystem::file_size(log_path);
    size_t record_num = file_size / (sizeof(uint8_t) + 2 * sizeof(int));

    std::vector<Record> records(record_num);
    int64_t current = 0;
    for (size_t k = 0; k != record_num; ++k) {
        uint8_t type;
        int start_32, end_32;
        fread(&type, sizeof(uint8_t), 1, fin);
        fread(&start_32, sizeof(int), 1, fin);
        fread(&end_32, sizeof(int), 1, fin);

        records[k].type = static_cast<int>(type);
        current += start_32;
        records[k].start = current;
        current += end_32;
        records[k].end = current;
    }

    return records;
}

size_t warmup_count(const std::vector<Record>& records) {
    size_t record_num = records.size();
    int64_t max_span = 0;
    size_t max_span_idx = 0;
    for (size_t i = 1; i < record_num; ++i) {
        int64_t span = records[i].start - records[i - 1].end;
        if (span > max_span) {
            max_span = span;
            max_span_idx = i;
        }
    }

    return max_span_idx;
}

double get_seconds(int64_t microsecods) {
    return static_cast<double>(microsecods) / 1000000;
}

#endif //LDBC_ANALYZE_UTILS_H
