#ifndef LOGGER_H
#define LOGGER_H

#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

enum class LogType {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};

struct LogEntry {
    LogType type;
    std::string message;
};

class Logger {
    public:
        static std::vector<LogEntry> messages;
        static void Log(const std::string& message);
        static void Err(const std::string& message);

    private:
        static const std::string printTimeStamp();
        static void logHelper(const std::string& msg, LogType logType);
        static const std::string GREEN;
        static const std::string RED;
        static const std::string RESET;
};

#endif