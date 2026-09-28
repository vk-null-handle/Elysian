#pragma once
#include "defines.h"
#include <stdint.h>

void mem_init(void);
void mem_shutdown(void);

void mem_free(void* chunk, u64 size);
void* mem_alloc(u64);
void* mem_copy(void* dest, const void* source, u64 size);
void* mem_zero(void* chunk, u64 size);
void* mem_calloc(u64 count, u64 size);
void* mem_realloc(void* chunk, u64 size, u64 newsize);
void* mem_set(void* dest, i32 value, u64 size);

void mem_print_usage(void);
