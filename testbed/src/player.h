#pragma once

typedef struct player {
  const char* name;
  int health;
} Player;

void player_init(Player *player);
void player_tick(Player *player);
void player_destroy(Player *player);
