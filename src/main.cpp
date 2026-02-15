#include <iostream>
#include <filesystem>
#include <vector>
#include <boost/program_options.hpp>
#include <spdlog/spdlog.h>
#include <format>

#include "config_parser.hpp"
#include "csv_reader.hpp"
#include "median_calculator.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::info);

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("cfg", po::value<std::string>(), "path to config.toml")
        ("config", po::value<std::string>(), "path to config.toml");

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (const std::exception &e) {
        spdlog::error("Failed to parse arguments: {}", e.what());
        return 1;
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    std::filesystem::path cfg_path;
    if (vm.count("cfg")) cfg_path = vm["cfg"].as<std::string>();
    else if (vm.count("config")) cfg_path = vm["config"].as<std::string>();
    else cfg_path = std::filesystem::current_path() / "config.toml";

    spdlog::info("Reading configuration: {}", cfg_path.string());

    auto cfg_opt = parse_config(cfg_path);
    if (!cfg_opt) return 2;
    AppConfig cfg = *cfg_opt;

    spdlog::info("Input dir: {}", cfg.input_dir.string());
    spdlog::info("Output dir: {}", cfg.output_dir.string());

    std::vector<DataRow> rows;
    if (!collect_rows_from_dir(cfg.input_dir, cfg.filename_mask, rows)) {
        spdlog::error("Failed to collect rows from input dir");
        return 3;
    }

    spdlog::info("Total rows to process: {}", rows.size());

    MedianCalculator calc(cfg.output_dir);
    calc.process_rows(rows);

    spdlog::info("Processing complete. Output: {}/median_result.csv", cfg.output_dir.string());
    return 0;
}
