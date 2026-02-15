// Lightweight config parser wrapper using toml++
#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <optional>
#include <toml++/toml.h>
#include <spdlog/spdlog.h>

struct AppConfig {
    std::filesystem::path input_dir;
    std::filesystem::path output_dir;
    std::vector<std::string> filename_mask;
};

inline std::optional<AppConfig> parse_config(const std::filesystem::path &cfg_path) {
    try {
        auto doc = toml::parse_file(cfg_path.string());
        auto main = doc["main"];
        if (!main || !main.is_table()) {
            spdlog::error("config: missing or invalid [main] table");
            return std::nullopt;
        }

        auto input_node = main["input"];
        if (!input_node || !input_node.is_string()) {
            spdlog::error("config: 'input' is required and must be a string");
            return std::nullopt;
        }

        AppConfig cfg;
        cfg.input_dir = input_node.value<std::string>().value();

        auto output_node = main["output"];
        if (output_node && output_node.is_string()) {
            cfg.output_dir = output_node.value<std::string>().value();
        } else {
            cfg.output_dir = std::filesystem::current_path() / "output";
        }

        auto masks_node = main["filename_mask"];
        if (masks_node && masks_node.is_array()) {
            for (auto &v : *masks_node.as_array()) {
                if (v.is_string()) cfg.filename_mask.emplace_back(v.value<std::string>().value());
            }
        }

        return cfg;
    } catch (const toml::parse_error &e) {
        spdlog::error("Failed to parse config {}: {}", cfg_path.string(), e.description());
        return std::nullopt;
    }
}
