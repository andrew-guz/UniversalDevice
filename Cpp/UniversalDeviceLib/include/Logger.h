#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <string>
#include <map>
#include <fstream>
#include <iostream>

#include "PathHelper.h"
#include "TimeHelper.h"

class FileStreamWrapper final
{
protected:
    FileStreamWrapper() = default;

public:
    ~FileStreamWrapper() = default;

    static std::ostream& Stream(const std::string& path);

private:
    static std::map<std::string, std::fstream*> _fileStreams;
};

enum class LogLevel
{
    INFO,
    ERROR
};

class Logger final
{
protected:
    Logger(LogLevel logLevel, const std::string& path);

public:
    ~Logger() = default;

    static Logger& Instance(LogLevel logLevel, const std::string& path);

    template <typename T>
    std::ostream& operator<<(const T& data)
    {
        auto& stream = FileStreamWrapper::Stream(_logPath);
        stream << TimeHelper::TimeToString(std::chrono::system_clock::now()) << "\t";
        switch (_logLevel)
        {
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
    static std::map<LogLevel, Logger*>  _instanceMap;
    LogLevel                            _logLevel;
    std::string                         _logPath;
};

#define LOG_INFO Logger::Instance(LogLevel::INFO, PathHelper::AppLogPath())
#define LOG_ERROR Logger::Instance(LogLevel::ERROR, PathHelper::AppLogPath())

#endif // _LOGGER_H_
