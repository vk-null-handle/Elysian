#include "memory.h"
#include "core/logger/logger.h"
#include <stdlib.h>
#include <string.h>

struct memory_stats {
  u64 total_allocated;
};

static struct memory_stats stats;

void mem_init(void) { mem_zero(&stats, sizeof(stats)); }
void mem_shutdown(void) {}

void mem_free(void *chunk, u64 size) {
  free(chunk);
  stats.total_allocated -= size;
  LOG_INFO(MEMORY, "Freed %llu bytes from address: %p", size, chunk);
}

void *mem_alloc(u64 size) {
  void *chunk = malloc(size);
  if (!chunk) {
    return NULL;
  }

  memset(chunk, 0, size);

  stats.total_allocated += size;
  LOG_DEBUG(MEMORY, "Allocated %llu bytes at address: %p", size, chunk);
  return chunk;
}

void *mem_copy(void *dest, const void *source, u64 size) {
  return memcpy(dest, source, size);
}

void *mem_zero(void *chunk, u64 size) { return memset(chunk, 0, size); }

void *mem_realloc(void *chunk, u64 size, u64 newsize) {
  return realloc(chunk, newsize);
  stats.total_allocated -= size;
  stats.total_allocated += newsize;
}

void *mem_calloc(u64 count, u64 size) {
  u64 total = count * size;

  void *chunk = malloc(total);
  if (!chunk) {
    return NULL;
  }

  memset(chunk, 0, total);

  stats.total_allocated += total;
  LOG_DEBUG(MEMORY, "Allocated %llu bytes at address: %p", total, chunk);
  return chunk;
}

void *mem_set(void *dest, i32 value, u64 size) {
  return memset(dest, value, size);
}

void mem_print_usage(void) {
  f64 value = (double)stats.total_allocated;
  const char *unit = "B";

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

  LOG_INFO(MEMORY, "System memory in use: %.2f %s", value, unit);
}
