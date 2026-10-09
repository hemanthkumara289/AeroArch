
#pragma once

#include <string>

namespace aeroarch::system
{
    struct SystemInfo
    {
        std::string hostname;
        std::string kernel_version;
        std::string architecture;
    };

    SystemInfo get_system_info();
}
