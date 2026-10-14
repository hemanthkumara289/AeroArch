
#include <exception>
#include <filesystem>
#include <iostream>

#include "aeroarch/core/config.hpp"
#include "aeroarch/core/logger.hpp"
#include "aeroarch/core/version.hpp"
#include "aeroarch/system/system_info.hpp"
#include "aeroarch/system/system_info_format.hpp"

namespace aeroarch
{
void print_welcome(const core::Config& config)
{
    const auto info = system::get_system_info();

    std::cout << "====================================\n";
    std::cout << "          " << VERSION_NAME << " OS\n";
    std::cout << "====================================\n";

    std::cout << "Version: " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH
              << '\n';

    std::cout << system::format_system_info(info, config.show_cpu, config.show_memory);

    std::cout << "Status: Development\n";
    std::cout << "====================================\n";
}
} // namespace aeroarch

int main()
{
    using aeroarch::core::Logger;
    using aeroarch::core::LogLevel;

    try
    {
        const auto config = aeroarch::core::load_config("config/aeroarch.ini");

        auto& logger = Logger::instance();

        LogLevel minimum_level;

        if (config.log_level == "DEBUG")
        {
            minimum_level = LogLevel::Debug;
        }
        else if (config.log_level == "WARNING")
        {
            minimum_level = LogLevel::Warning;
        }
        else if (config.log_level == "ERROR")
        {
            minimum_level = LogLevel::Error;
        }
        else
        {
            minimum_level = LogLevel::Info;
        }

        logger.set_minimum_level(minimum_level);

        logger.initialize(std::filesystem::path("logs") / "aeroarch.log");

        logger.info("AeroArch startup initiated");
        logger.debug("Configuration loaded successfully");

        aeroarch::print_welcome(config);

        logger.info("AeroArch startup completed");
    }
    catch (const std::exception& error)
    {
        std::cerr << "AeroArch error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
