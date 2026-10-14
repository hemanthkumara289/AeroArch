
#pragma once

#include <filesystem>
#include <string>

namespace aeroarch::core
{
struct Config
{
    std::string log_level = "INFO";

    bool show_cpu = true;
    bool show_memory = true;
};

Config load_config(const std::filesystem::path& config_file);
} // namespace aeroarch::core
