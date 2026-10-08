#include "player.h"
#include <stdio.h>

void player_init(Player *player) {
  player->name = "Player";
  player->health = 100;

  printf("[Game] Player created\n");
}

void player_tick(Player *player) { printf("[Game] Player tick\n"); }

void player_destroy(Player *player) { printf("[Game] Player destroyed\n"); }
