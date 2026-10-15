
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

[[noreturn]] void config_error(const std::filesystem::path& file, std::size_t line_number,
                               const std::string& message)
{
    throw std::runtime_error(file.string() + ":" + std::to_string(line_number) + ": " + message);
}
} // namespace

Config load_config(const std::filesystem::path& config_file)
{
    Config config;
    std::ifstream file(config_file);

    // Missing files use defaults, but other open failures are errors.
    if (!file.is_open())
    {
        std::error_code error;
        const bool exists = std::filesystem::exists(config_file, error);

        if (error || exists)
        {
            throw std::runtime_error("Cannot open configuration file: " + config_file.string());
        }

        return config;
    }

    std::string line;
    std::string section;
    std::size_t line_number = 0;

    while (std::getline(file, line))
    {
        ++line_number;
        line = trim(line);

        if (line.empty() || line[0] == '#' || line[0] == ';')
        {
            continue;
        }

        if (line.front() == '[')
        {
            if (line.back() != ']' || line.size() < 3)
            {
                config_error(config_file, line_number, "Malformed section header");
            }

            section = to_upper(trim(line.substr(1, line.size() - 2)));

            if (section != "LOGGING" && section != "SYSTEM")
            {
                config_error(config_file, line_number, "Unknown section: " + section);
            }

            continue;
        }

        const auto separator = line.find('=');

        if (separator == std::string::npos)
        {
            config_error(config_file, line_number, "Expected KEY=VALUE");
        }

        const std::string key = to_upper(trim(line.substr(0, separator)));

        const std::string value = trim(line.substr(separator + 1));

        if (key.empty() || value.empty())
        {
            config_error(config_file, line_number, "Setting name and value must not be empty");
        }

        try
        {
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
            else
            {
                config_error(config_file, line_number,
                             "Unknown setting '" + key + "' in section [" + section + "]");
            }
        }
        catch (const std::runtime_error& error)
        {
            const std::string message = error.what();

            if (message.find(config_file.string() + ":") == 0)
            {
                throw;
            }

            config_error(config_file, line_number, message);
        }
    }

    if (file.bad())
    {
        throw std::runtime_error("Error reading configuration file: " + config_file.string());
    }

    return config;
}
} // namespace aeroarch::core
