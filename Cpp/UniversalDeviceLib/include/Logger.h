#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <string>
#include <map>
#include <fstream>
#include <iostream>

#include "PathHelper.h"

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
    std::ostream& operator<<(const T& data_)
    {
        return FileStreamWrapper::Stream("");
    }

private:
    static std::map<LogLevel, Logger*>  _instanceMap;
    LogLevel                            _logLevel;
    std::string                         _logPath;
};

#define LOG_INFO Logger::Instance(LogLevel::INFO, "?")
#define LOG_ERROR Logger::Instance(LogLevel::ERROR, "?")

#endif // _LOGGER_H_
