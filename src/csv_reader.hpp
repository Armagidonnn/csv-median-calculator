// Simple CSV reader for required columns
#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <optional>
#include <algorithm>
#include <cctype>
#include <spdlog/spdlog.h>

struct DataRow {
    uint64_t receive_ts;
    long double price;
};

inline std::string trim(const std::string &s) {
    auto b = s.find_first_not_of(" \t\r\n");
    if (b==std::string::npos) return {};
    auto e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e-b+1);
}

inline bool file_matches_masks(const std::string &name, const std::vector<std::string> &masks) {
    if (masks.empty()) return true;
    for (auto &m : masks) {
        if (name.find(m) != std::string::npos) return true;
    }
    return false;
}

inline bool read_csv_file(const std::filesystem::path &path, std::vector<DataRow> &out) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        spdlog::error("Failed to open file {}", path.string());
        return false;
    }

    std::string header;
    if (!std::getline(ifs, header)) return true; // empty file

    std::vector<std::string> cols;
    {
        std::stringstream ss(header);
        std::string item;
        while (std::getline(ss, item, ';')) cols.push_back(trim(item));
    }

    int idx_receive = -1, idx_price = -1;
    for (size_t i=0;i<cols.size();++i) {
        if (cols[i]=="receive_ts") idx_receive = (int)i;
        if (cols[i]=="price") idx_price = (int)i;
    }
    if (idx_receive<0 || idx_price<0) {
        spdlog::error("File {} missing required columns", path.string());
        return false;
    }

    std::string line;
    size_t lineno = 1;
    while (std::getline(ifs, line)) {
        ++lineno;
        if (line.empty()) continue;
        std::vector<std::string> fields;
        std::stringstream ss(line);
        std::string item;
        while (std::getline(ss, item, ';')) fields.push_back(trim(item));

        if ((int)fields.size() <= std::max(idx_receive, idx_price)) {
            spdlog::warn("{}:{}: malformed CSV line, skipping", path.string(), lineno);
            continue;
        }

        try {
            uint64_t ts = std::stoull(fields[idx_receive]);
            long double price = std::stold(fields[idx_price]);
            out.push_back({ts, price});
        } catch (const std::exception &e) {
            spdlog::warn("{}:{}: invalid numeric value: {}", path.string(), lineno, e.what());
            continue;
        }
    }

    return true;
}

inline bool collect_rows_from_dir(const std::filesystem::path &dir, const std::vector<std::string> &masks, std::vector<DataRow> &out) {
    if (!std::filesystem::exists(dir)) {
        spdlog::error("Input directory does not exist: {}", dir.string());
        return false;
    }
    size_t files_found = 0;
    for (auto &p : std::filesystem::directory_iterator(dir)) {
        if (!p.is_regular_file()) continue;
        auto name = p.path().filename().string();
        if (!file_matches_masks(name, masks)) continue;
        if (p.path().extension() != ".csv") continue;
        ++files_found;
        spdlog::info("Reading file: {}", p.path().string());
        if (!read_csv_file(p.path(), out)) {
            spdlog::warn("Failed to read file {}", p.path().string());
        }
    }
    spdlog::info("Found files: {}; total rows collected: {}", files_found, out.size());
    return true;
}
