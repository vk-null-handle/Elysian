#pragma once
#include "defines.h"

// Opaque pointer
typedef struct engine Engine;

Engine* engine_init(void);
void engine_shutdown(Engine* engine);
void engine_run(Engine* engine);
