
#pragma once

#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>

namespace aeroarch::core
{
enum class LogLevel
{
    Debug,
    Info,
    Warning,
    Error
};

class Logger
{
  public:
    static Logger& instance();

    void initialize(const std::filesystem::path& log_file);

    void set_minimum_level(LogLevel level);

    void log(LogLevel level, std::string_view message);

    void debug(std::string_view message);
    void info(std::string_view message);
    void warning(std::string_view message);
    void error(std::string_view message);

    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

  private:
    Logger() = default;

    std::string current_timestamp() const;
    std::string level_name(LogLevel level) const;

    std::ofstream log_file_;
    LogLevel minimum_level_ = LogLevel::Debug;
    std::mutex mutex_;
};
} // namespace aeroarch::core
