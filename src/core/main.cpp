
#include <iostream>
#include <stdexcept>

#include "aeroarch/core/version.hpp"
#include "aeroarch/system/system_info.hpp"

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

    std::cout << "Hostname: " << info.hostname << '\n';
    std::cout << "Kernel: " << info.kernel_version << '\n';
    std::cout << "Architecture: " << info.architecture << '\n';

    std::cout << "CPU: " << info.cpu_model << '\n';
    std::cout << "Logical CPUs: " << info.logical_cpu_count << '\n';

    std::cout << "Total memory: " << info.total_memory_kb / 1024 << " MiB\n";

    std::cout << "Available memory: " << info.available_memory_kb / 1024 << " MiB\n";

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
