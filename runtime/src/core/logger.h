#pragma once 

typedef enum log_severity {
    LOG_SEVERITY_TRACE,
    LOG_SEVERITY_INFO,
    LOG_SEVERITY_DEBUG,
    LOG_SEVERITY_WARN,
    LOG_SEVERITY_ERROR,
    LOG_SEVERITY_FATAL
} LogSeverity;

typedef enum log_tag {
    VULKAN,
    DX12,
    RENDERER,
    MEMORY,
    PLATFORM,
    ENGINE,
    OTHER,
} LogTag;

void log_output(LogSeverity severity, LogTag tag, const char* msg, ...);

#ifdef ENABLE_TRACE
#define LOG_TRACE(system, ...) log_output(LOG_SEVERITY_TRACE, system, __VA_ARGS__)
#else
#define LOG_TRACE(system, ...) ((void)0)
#endif

#ifdef DEBUG
#define LOG_DEBUG(system, ...) log_output(LOG_SEVERITY_DEBUG, system, __VA_ARGS__)
#else
#define LOG_DEBUG(system, ...) ((void)0)
#endif

#define LOG_INFO(system, ...)  log_output(LOG_SEVERITY_INFO,  system, __VA_ARGS__)
#define LOG_WARN(system, ...)  log_output(LOG_SEVERITY_WARN,  system, __VA_ARGS__)
#define LOG_ERROR(system, ...) log_output(LOG_SEVERITY_ERROR, system, __VA_ARGS__)
#define LOG_FATAL(system, ...) log_output(LOG_SEVERITY_FATAL, system, __VA_ARGS__)

