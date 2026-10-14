
#include <iostream>
#include <stdexcept>

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
    try
    {
        aeroarch::print_welcome();
    }
    catch (const std::exception& error)
    {
        std::cerr << "AeroArch error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
