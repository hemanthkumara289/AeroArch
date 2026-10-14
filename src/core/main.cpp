
#include <exception>
#include <filesystem>
#include <iostream>

#include "aeroarch/core/logger.hpp"
#include "aeroarch/core/version.hpp"
#include "aeroarch/system/system_info.hpp"
#include "aeroarch/system/system_info_format.hpp"

namespace aeroarch
{
void print_welcome()
{
    const auto info = system::get_system_info();

    std::cout << "====================================\n";
    std::cout << "          " << VERSION_NAME << " OS\n";
    std::cout << "====================================\n";

    std::cout << "Version: " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH
              << '\n';

    std::cout << system::format_system_info(info);

    std::cout << "Status: Development\n";
    std::cout << "====================================\n";
}
} // namespace aeroarch

int main()
{
    using aeroarch::core::Logger;

    try
    {
        auto& logger = Logger::instance();
        logger.set_minimum_level(aeroarch::core::LogLevel::Info);

        logger.initialize(std::filesystem::path("logs") / "aeroarch.log");

        logger.info("AeroArch startup initiated");
        logger.debug("Collecting system information");

        aeroarch::print_welcome();

        logger.info("AeroArch startup completed");
    }
    catch (const std::exception& error)
    {
        std::cerr << "AeroArch error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
