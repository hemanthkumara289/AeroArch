
#include "aeroarch/system/system_info.hpp"

#include <fstream>
#include <limits.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/utsname.h>
#include <unistd.h>

namespace aeroarch::system
{
SystemInfo get_system_info()
{
    SystemInfo info;

    // Hostname
    char hostname_buffer[HOST_NAME_MAX + 1] = {};

    if (gethostname(hostname_buffer, sizeof(hostname_buffer)) != 0)
    {
        throw std::runtime_error("Failed to retrieve hostname");
    }

    hostname_buffer[HOST_NAME_MAX] = '\0';
    info.hostname = hostname_buffer;

    // Kernel version and architecture
    struct utsname system_data
    {
    };

    if (uname(&system_data) != 0)
    {
        throw std::runtime_error("Failed to retrieve kernel information");
    }

    info.kernel_version = system_data.release;
    info.architecture = system_data.machine;

    // Logical CPU count
    const long cpu_count = sysconf(_SC_NPROCESSORS_ONLN);

    if (cpu_count > 0)
    {
        info.logical_cpu_count = static_cast<unsigned int>(cpu_count);
    }

    // CPU model from /proc/cpuinfo
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;

    while (std::getline(cpuinfo, line))
    {
        const auto separator = line.find(':');

        if (separator == std::string::npos)
        {
            continue;
        }

        const std::string key = line.substr(0, separator);

        if (key == "model name" || key == "Hardware")
        {
            info.cpu_model = line.substr(separator + 1);

            const auto first = info.cpu_model.find_first_not_of(" \t");

            if (first != std::string::npos)
            {
                info.cpu_model.erase(0, first);
            }

            break;
        }
    }

    // Total and available memory from /proc/meminfo
    std::ifstream meminfo("/proc/meminfo");

    while (std::getline(meminfo, line))
    {
        std::istringstream input(line);

        std::string key;
        unsigned long long value = 0;
        std::string unit;

        if (!(input >> key >> value >> unit))
        {
            continue;
        }

        if (key == "MemTotal:")
        {
            info.total_memory_kb = value;
        }
        else if (key == "MemAvailable:")
        {
            info.available_memory_kb = value;
        }

        if (info.total_memory_kb != 0 && info.available_memory_kb != 0)
        {
            break;
        }
    }

    return info;
}
} // namespace aeroarch::system
