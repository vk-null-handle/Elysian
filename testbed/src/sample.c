#include "game.h"
#include "player.h"

// Game objects
Player player;

void game_init() {
	player_init(&player);
}

void game_tick() {
	player_tick(&player);
}

void game_shutdown() {
	player_destroy(&player);
}
