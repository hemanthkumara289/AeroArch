
#include "aeroarch/core/config.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <stdexcept>
#include <string>

namespace aeroarch::core
{
namespace
{
std::string trim(const std::string& value)
{
    const auto first = value.find_first_not_of(" \t\r\n");

    if (first == std::string::npos)
    {
        return "";
    }

    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::string to_upper(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char character)
                   { return static_cast<char>(std::toupper(character)); });

    return value;
}

bool parse_bool(const std::string& value)
{
    const std::string normalized = to_upper(value);

    if (normalized == "TRUE" || normalized == "1" || normalized == "YES")
    {
        return true;
    }

    if (normalized == "FALSE" || normalized == "0" || normalized == "NO")
    {
        return false;
    }

    throw std::runtime_error("Invalid boolean value: " + value);
}
} // namespace

Config load_config(const std::filesystem::path& config_file)
{
    Config config;
    std::ifstream file(config_file);

    // A missing configuration file uses the default settings.
    if (!file.is_open())
    {
        return config;
    }

    std::string line;
    std::string section;

    while (std::getline(file, line))
    {
        line = trim(line);

        if (line.empty() || line[0] == '#' || line[0] == ';')
        {
            continue;
        }

        if (line.front() == '[' && line.back() == ']')
        {
            section = to_upper(trim(line.substr(1, line.size() - 2)));
            continue;
        }

        const auto separator = line.find('=');

        if (separator == std::string::npos)
        {
            continue;
        }

        const std::string key = to_upper(trim(line.substr(0, separator)));
        const std::string value = trim(line.substr(separator + 1));

        if (section == "LOGGING" && key == "LEVEL")
        {
            const std::string level = to_upper(value);

            if (level != "DEBUG" && level != "INFO" && level != "WARNING" && level != "ERROR")
            {
                throw std::runtime_error("Invalid logging level: " + value);
            }

            config.log_level = level;
        }
        else if (section == "SYSTEM" && key == "SHOW_CPU")
        {
            config.show_cpu = parse_bool(value);
        }
        else if (section == "SYSTEM" && key == "SHOW_MEMORY")
        {
            config.show_memory = parse_bool(value);
        }
    }

    if (file.bad())
    {
        throw std::runtime_error("Error reading configuration file: " + config_file.string());
    }

    return config;
}
} // namespace aeroarch::core
