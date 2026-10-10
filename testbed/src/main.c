#include "engine.h"

int main() {
	// Create engine handle
	Engine* engine = engine_init();
	engine_run(engine);
	engine_shutdown(engine);
	return 0;
}
