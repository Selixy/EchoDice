#include "Logger.hpp"

#include <iostream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>

static LogLevel g_currentLevel = LogLevel::Debug;
static std::mutex g_logMutex;

/// Retourne la chaîne correspondante au niveau
static const char* levelToString(LogLevel lvl) {
    switch (lvl) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO ";
        case LogLevel::Warning: return "WARN ";
        case LogLevel::Error:   return "ERROR";
    }
    return "UNKWN";
}

void initLogger(LogLevel level) {
    std::lock_guard<std::mutex> lock(g_logMutex);
    g_currentLevel = level;
}

void logMessage(LogLevel level, const std::string& message) {
    if (level < g_currentLevel) return;

    // Timestamp
    auto now = std::chrono::system_clock::now();
    auto itt = std::chrono::system_clock::to_time_t(now);
    auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(
                   now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&itt), "%Y-%m-%d %H:%M:%S")
        << '.' << std::setfill('0') << std::setw(3) << ms.count()
        << " [" << levelToString(level) << "] "
        << message << '\n';

    std::lock_guard<std::mutex> lock(g_logMutex);
    std::cout << oss.str();
    std::cout.flush();
}
