
#include "aeroarch/system/system_info.hpp"

#include <fstream>
#include <stdexcept>
#include <string>
#include <sys/utsname.h>
#include <unistd.h>
#include <limits.h>

namespace aeroarch::system
{
    SystemInfo get_system_info()
    {
        SystemInfo info;

        char hostname_buffer[HOST_NAME_MAX + 1] = {};

        if (gethostname(hostname_buffer, sizeof(hostname_buffer)) != 0)
        {
            throw std::runtime_error("Failed to retrieve hostname");
        }

        info.hostname = hostname_buffer;

        struct utsname system_data {};

        if (uname(&system_data) != 0)
        {
            throw std::runtime_error("Failed to retrieve kernel information");
        }

        info.kernel_version = system_data.release;
        info.architecture = system_data.machine;

        return info;
    }
}
