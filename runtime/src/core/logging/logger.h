#pragma once
#include "defines.h"
#include <stdio.h>
#include <stdlib.h>

#define LOG_INFO(...)    \
	printf("[INFO]: ");  \
	printf(__VA_ARGS__); \
	printf("\n");

#define LOG_WARN(...)      \
	printf("[WARNING]: "); \
	printf(__VA_ARGS__);   \
	printf("\n");

#define LOG_ERROR(...)   \
	printf("[ERROR]: "); \
	printf(__VA_ARGS__); \
	printf("\n");        \
	getchar();

#define LOG_FATAL(...)   \
	printf("[FATAL]: "); \
	printf(__VA_ARGS__); \
	printf("\n");        \
	exit(1);

#define ASSERT(cond, message)                                                                                           \
	do {                                                                                                                \
		if (!cond) {                                                                                                    \
			LOG_FATAL("Assertion failure: %s, message: %s, in file: %s, line: %i", #cond, message, __FILE__, __LINE__); \
		}                                                                                                               \
	} while (0)\
