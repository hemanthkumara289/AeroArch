
#include "aeroarch/system/system_info_format.hpp"

#include <sstream>

namespace aeroarch::system
{
std::string format_system_info(const SystemInfo& info)
{
    std::ostringstream output;

    output << "Hostname: " << info.hostname << '\n';
    output << "Kernel: " << info.kernel_version << '\n';
    output << "Architecture: " << info.architecture << '\n';
    output << "CPU: " << info.cpu_model << '\n';
    output << "Logical CPUs: " << info.logical_cpu_count << '\n';
    output << "Total memory: " << info.total_memory_kb / 1024 << " MiB\n";
    output << "Available memory: " << info.available_memory_kb / 1024 << " MiB\n";

    return output.str();
}
} // namespace aeroarch::system
