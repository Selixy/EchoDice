#pragma once

#include <string>

/// Niveau de log
enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

/// Initialise le niveau minimal à logger (par défaut Debug)
void initLogger(LogLevel level = LogLevel::Debug);

/// Logue un message au niveau spécifié
void logMessage(LogLevel level, const std::string& message);

/// Macros pratiques
#define LOG_DEBUG(msg)   logMessage(LogLevel::Debug,   msg)
#define LOG_INFO(msg)    logMessage(LogLevel::Info,    msg)
#define LOG_WARNING(msg) logMessage(LogLevel::Warning, msg)
#define LOG_ERROR(msg)   logMessage(LogLevel::Error,   msg)
