#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "PathHelper.h"
#include "TimeHelper.h"

class FileStreamWrapper final {
protected:
    FileStreamWrapper() = default;

public:
    ~FileStreamWrapper() = default;

    static std::ostream& Stream(const std::string& path);

    static std::ostream& NullStream();

private:
    static std::map<std::string, std::fstream*> _fileStreams;
    static std::ostream _nullStream;
};

enum class LogLevel { DEBUG, INFO, ERROR };

class Logger final {
protected:
    Logger(LogLevel logLevel, const std::string& path);

public:
    ~Logger() = default;

    static Logger& Instance(LogLevel logLevel, const std::string& path);

    static void SetLogLevel(LogLevel minLogLevel);

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

#define LOG_DEBUG Logger::Instance(LogLevel::DEBUG, PathHelper::AppLogPath())
#define LOG_INFO Logger::Instance(LogLevel::INFO, PathHelper::AppLogPath())
#define LOG_ERROR Logger::Instance(LogLevel::ERROR, PathHelper::AppLogPath())

#endif // _LOGGER_H_
