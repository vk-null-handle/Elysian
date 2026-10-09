#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void log_output(LogSeverity severity, LogTag tag, const char* msg, ...) {
	char buff[1024];
	const char* severity_str;
	const char* system_str;

	va_list list;
	va_start(list, msg);
	vsnprintf(buff, sizeof(buff), msg, list);
	va_end(list);

	// Switch on severity
	switch (severity) {
	case LOG_SEVERITY_TRACE:
		severity_str = "TRACE";
		break;
	case LOG_SEVERITY_INFO:
		severity_str = "INFO";
		break;
	case LOG_SEVERITY_DEBUG:
		severity_str = "DEBUG";
		break;
	case LOG_SEVERITY_WARN:
		severity_str = "WARN";
		break;
	case LOG_SEVERITY_ERROR:
		severity_str = "ERROR";
		break;
	case LOG_SEVERITY_FATAL:
		severity_str = "FATAL";
		break;
	default:
		severity_str = "UNKNOWN";
		break;
	}

	// Switch on system
	switch (tag) {
	case VULKAN:
		system_str = "VULKAN";
		break;
	case DX12:
		system_str = "DIRECTX12";
		break;
	case RENDERER:
		system_str = "RENDERER";
		break;
	case PLATFORM:
		system_str = "PLATFROM";
		break;
	case MEMORY:
		system_str = "MEMORY";
		break;
	case ENGINE:
		system_str = "ENGINE";
		break;
	default:
		severity_str = "UNKNOWN";
		break;
	}

	printf("[%s] [%s] %s\n", severity_str, system_str, buff);

	if (severity == LOG_SEVERITY_FATAL) {
		exit(EXIT_FAILURE);
	}

	if (severity == LOG_SEVERITY_ERROR) {
		getchar();
	}
}
