// Median calculator using two heaps (incremental)
#pragma once

#include <queue>
#include <vector>
#include <fstream>
#include <iomanip>
#include <string>
#include <spdlog/spdlog.h>
#include "csv_reader.hpp"

class MedianCalculator {
public:
    MedianCalculator(const std::filesystem::path &out_dir) {
        std::filesystem::create_directories(out_dir);
        out_path_ = out_dir / "median_result.csv";
        // write header
        std::ofstream ofs(out_path_);
        ofs << "receive_ts;price_median\n";
    }

    void process_rows(std::vector<DataRow> &rows) {
        // sort by receive_ts
        std::sort(rows.begin(), rows.end(), [](auto &a, auto &b){
            if (a.receive_ts != b.receive_ts) return a.receive_ts < b.receive_ts;
            return a.price < b.price;
        });

        long double prev_median = std::numeric_limits<long double>::quiet_NaN();

        for (auto &r : rows) {
            add_value(r.price);
            long double med = current_median();
            if (!std::isfinite(prev_median) || format_med(prev_median) != format_med(med)) {
                write_entry(r.receive_ts, med);
                prev_median = med;
            }
        }
    }

private:
    std::priority_queue<long double> lower_; // max-heap
    std::priority_queue<long double, std::vector<long double>, std::greater<long double>> upper_; // min-heap
    std::filesystem::path out_path_;

    void add_value(long double v) {
        if (lower_.empty() || v <= lower_.top()) lower_.push(v);
        else upper_.push(v);

        if ((int)lower_.size() > (int)upper_.size() + 1) {
            upper_.push(lower_.top()); lower_.pop();
        } else if ((int)upper_.size() > (int)lower_.size() + 1) {
            lower_.push(upper_.top()); upper_.pop();
        }
    }

    long double current_median() const {
        if (lower_.size() == upper_.size()) {
            if (lower_.empty()) return 0.0L;
            return (lower_.top() + upper_.top()) / 2.0L;
        }
        return (lower_.size() > upper_.size()) ? lower_.top() : upper_.top();
    }

    static std::string format_med(long double v) {
        std::ostringstream ss;
        ss.setf(std::ios::fixed); ss<<std::setprecision(8)<< (double)v;
        return ss.str();
    }

    void write_entry(uint64_t ts, long double med) {
        std::ofstream ofs(out_path_, std::ios::app);
        if (!ofs.is_open()) {
            spdlog::error("Failed to open output file {}", out_path_.string());
            return;
        }
        ofs << ts << ";" << format_med(med) << "\n";
        spdlog::info("Median changed: {} => {}", ts, format_med(med));
    }
};
