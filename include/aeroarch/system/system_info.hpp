
#pragma once

#include <string>

namespace aeroarch::system
{
    struct SystemInfo
    {
        std::string hostname;
        std::string kernel_version;
        std::string architecture;

        std::string cpu_model;
        unsigned int logical_cpu_count = 0;

        unsigned long long total_memory_kb = 0;
        unsigned long long available_memory_kb = 0;
    };

    SystemInfo get_system_info();
}
