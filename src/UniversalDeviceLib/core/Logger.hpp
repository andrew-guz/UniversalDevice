#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <string>

#include "PathHelper.hpp"
#include "TimeHelper.hpp"

class FileStreamWrapper final {
protected:
    FileStreamWrapper() = default;

public:
    ~FileStreamWrapper() = default;

    static std::ostream& Stream(const std::filesystem::path& path);

    static std::ostream& NullStream();

    static void Cleanup();

private:
    static std::map<std::filesystem::path, std::fstream*> _fileStreams;
    static std::ostream _nullStream;
    static std::mutex _mutex;
};

enum class LogLevel { DEBUG, INFO, ERROR };

class Logger final {
protected:
    Logger(LogLevel logLevel, const std::filesystem::path& path);

public:
    ~Logger() = default;

    static Logger& Instance(LogLevel logLevel, const std::filesystem::path& path);

    static void SetLogLevel(LogLevel minLogLevel);

    static void Cleanup();

    template<typename T>
    std::ostream& operator<<(const T& data) {
        auto& stream = _logLevel >= _minLogLevel ? FileStreamWrapper::Stream(_logPath) : FileStreamWrapper::NullStream();
        stream << TimeHelper::TimeToString(std::chrono::system_clock::now()) << "\t";
        switch (_logLevel) {
            case LogLevel::DEBUG:
                stream << "[DEBUG]";
                break;
            case LogLevel::INFO:
                stream << "[INFO]";
                break;
            case LogLevel::ERROR:
                stream << "[ERROR]";
                break;
        }
        stream << "\t" << data;
        return stream;
    }

private:
    static std::map<LogLevel, Logger*> _instanceMap;
    LogLevel _logLevel;
    static LogLevel _minLogLevel;
    std::string _logPath;
};

#define LOG(level)         Logger::Instance(level, PathHelper::AppLogPath())

#define LOG_DEBUG          LOG(LogLevel::DEBUG)
#define LOG_INFO           LOG(LogLevel::INFO)
#define LOG_ERROR          LOG(LogLevel::ERROR)

#define LOG_DEBUG_MSG(msg) LOG_DEBUG << (msg) << std::endl
#define LOG_INFO_MSG(msg)  LOG_INFO << (msg) << std::endl
#define LOG_ERROR_MSG(msg) LOG_ERROR << (msg) << std::endl
