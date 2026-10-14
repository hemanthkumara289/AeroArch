
#include "aeroarch/core/logger.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace aeroarch::core
{
Logger& Logger::instance()
{
    static Logger logger;
    return logger;
}

Logger::~Logger()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (log_file_.is_open())
    {
        log_file_.flush();
        log_file_.close();
    }
}

void Logger::initialize(const std::filesystem::path& log_file)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (log_file_.is_open())
    {
        log_file_.flush();
        log_file_.close();
    }

    if (!log_file.parent_path().empty())
    {
        std::filesystem::create_directories(log_file.parent_path());
    }

    log_file_.open(log_file, std::ios::out | std::ios::app);

    if (!log_file_.is_open())
    {
        throw std::runtime_error("Failed to open AeroArch log file");
    }
}

void Logger::set_minimum_level(LogLevel level)
{
    std::lock_guard<std::mutex> lock(mutex_);
    minimum_level_ = level;
}

std::string Logger::current_timestamp() const
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::tm local_time{};

    if (localtime_r(&time, &local_time) == nullptr)
    {
        return "unknown-time";
    }

    std::ostringstream timestamp;
    timestamp << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S");

    return timestamp.str();
}

std::string Logger::level_name(LogLevel level) const
{
    switch (level)
    {
    case LogLevel::Debug:
        return "DEBUG";

    case LogLevel::Info:
        return "INFO";

    case LogLevel::Warning:
        return "WARN";

    case LogLevel::Error:
        return "ERROR";
    }

    return "UNKNOWN";
}

void Logger::log(LogLevel level, std::string_view message)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (static_cast<int>(level) < static_cast<int>(minimum_level_))
    {
        return;
    }

    const std::string entry =
        "[" + current_timestamp() + "] [" + level_name(level) + "] " + std::string(message);

    std::clog << entry << '\n';

    if (log_file_.is_open())
    {
        log_file_ << entry << '\n';
        log_file_.flush();

        if (!log_file_)
        {
            throw std::runtime_error("Failed to write AeroArch log file");
        }
    }
}

void Logger::debug(std::string_view message)
{
    log(LogLevel::Debug, message);
}

void Logger::info(std::string_view message)
{
    log(LogLevel::Info, message);
}

void Logger::warning(std::string_view message)
{
    log(LogLevel::Warning, message);
}

void Logger::error(std::string_view message)
{
    log(LogLevel::Error, message);
}
} // namespace aeroarch::core
