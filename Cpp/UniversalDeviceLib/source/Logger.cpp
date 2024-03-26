#include "Logger.h"

std::map<std::string, std::fstream*> FileStreamWrapper::_fileStreams;
std::ostream FileStreamWrapper::_nullStream(0);

std::ostream& FileStreamWrapper::Stream(const std::string& path)
{
    if (_fileStreams.count(path) == 0)
    {
        auto fileStream = new std::fstream();
        fileStream->open(path, std::fstream::out | std::fstream::app);
        _fileStreams.insert(std::make_pair(path, fileStream));
    }
    return *_fileStreams[path];
}

std::ostream& FileStreamWrapper::NullStream()
{
    return _nullStream;
}

std::map<LogLevel, Logger*>  Logger::_instanceMap;
LogLevel Logger::_minLogLevel = LogLevel::DEBUG;

Logger::Logger(LogLevel logLevel, const std::string& path) :
    _logLevel(logLevel),
    _logPath(path)
{

}

Logger& Logger::Instance(LogLevel logLevel, const std::string& path)
{    
    if (_instanceMap.count(logLevel) == 0)
        _instanceMap.insert(std::make_pair(logLevel, new Logger(logLevel, path)));
    return *_instanceMap[logLevel];
}

void Logger::SetLogLevel(LogLevel minLogLevel)
{
    _minLogLevel = minLogLevel;
}
