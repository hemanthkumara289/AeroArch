
#pragma once

#include "aeroarch/system/system_info.hpp"

#include <string>

namespace aeroarch::system
{
std::string format_system_info(const SystemInfo& info, bool show_cpu = true,
                               bool show_memory = true);
}
