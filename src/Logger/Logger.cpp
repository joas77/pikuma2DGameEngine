#include "Logger.h"

std::vector<LogEntry> Logger::messages;

const std::string Logger::GREEN = "\033[32m";
const std::string Logger::RED = "\033[31m";
const std::string Logger::RESET = "\033[0m";

void Logger::Log(const std::string& message) {
    logHelper(message, LogType::LOG_INFO);
}

void Logger::Err(const std::string& message) {
    logHelper(message, LogType::LOG_ERROR);
}

void Logger::logHelper(const std::string& message, LogType logType) {
    LogEntry logEntry;
    logEntry.type = logType;
    std::string color;
    std::string logDesc;

    switch (logType)
    {
    case LogType::LOG_ERROR:
        color = RED;
        logDesc = "ERR";
        break;
    case LogType::LOG_INFO:
        color = GREEN;
        logDesc = "LOG";
        break;
    default:
        break;
    }

    logEntry.message = logDesc + " [ " + printTimeStamp() + " ] - " + message;
    std::cout << color << logEntry.message << RESET << std::endl;
    messages.push_back(logEntry);
}

const std::string Logger::printTimeStamp() {
    // get current time
    time_t now = std::time(nullptr);
    
    // format timestamp string
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}