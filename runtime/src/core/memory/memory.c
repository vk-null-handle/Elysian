#include "memory.h"
#include "core/logging/logger.h"
#include "core/platform/platform.h"

struct memory_stats {
	u64 total_allocated;
};

static struct memory_stats stats;

void mem_init(void) {
	mem_zero(&stats, sizeof(stats));
}
void mem_shutdown(void) {
}

void mem_free(void* chunk, u64 size) {
	LOG_INFO("Memory: freed %llu bytes from address: %p", size, chunk);
	stats.total_allocated -= size;
	platform_free(chunk, FALSE);
}

void* mem_alloc(u64 size) {
	void* chunk = platform_allocate(size, FALSE);
	if (!chunk) {
		return NULL;
	}

	stats.total_allocated += size;
	LOG_INFO("Memory: allocated %llu bytes at address: %p", size, chunk);

	mem_zero(chunk, size);
	return chunk;
}

void* mem_copy(void* dest, const void* source, u64 size) {
	return platform_copy_memory(dest, source, size);
}

void* mem_zero(void* chunk, u64 size) {
	return platform_zero_memory(chunk, size);
}

void* mem_realloc(void* chunk, u64 size, u64 newsize) {
	stats.total_allocated -= size;
	stats.total_allocated += newsize;
	return platform_reallocate(chunk, newsize, FALSE);
}

void* mem_calloc(u64 count, u64 size) {
	u64 total = count * size;

	void* chunk = platform_allocate(total, FALSE);
	if (!chunk) {
		return NULL;
	}

	stats.total_allocated += total;
	LOG_INFO("Memory: allocated %llu bytes at address: %p", total, chunk);
	platform_zero_memory(chunk, total);

	return chunk;
}

void* mem_set(void* dest, i32 value, u64 size) {
	return platform_set_memory(dest, value, size);
}

void mem_print_usage(void) {
	f64 value = (double)stats.total_allocated;
	const char* unit = "B";

	if (value >= 1024.0) {
		value /= 1024.0;
		unit = "KiB";
	}
	if (value >= 1024.0) {
		value /= 1024.0;
		unit = "MiB";
	}
	if (value >= 1024.0) {
		value /= 1024.0;
		unit = "GiB";
	}

	LOG_INFO("System memory in use: %.2f %s", value, unit);
}
