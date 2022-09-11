#pragma once
#include <SFML/Graphics.hpp>

/* General Game Layout info */
#define TILE_SCALING 3
#define TILES_X 24
#define TILES_Y 12
#define GENERATE_MAP true

/* Player Info UI element width in tiles */
#define INFO_WIDTH	7
#define UI_PLAYER_SEGMENT_HEIGHT 115

/* Gamplay Properties */
#define SPAWN_AREA_SIZE 2
#define MOVEMENT_SPEED 100.f
#define EXPLOSION_LIFETIME 0.5f
#define EXPLOSION_THRESHOLD_1 0.5f
#define EXPLOSION_THRESHOLD_2 0.25f
#define EXPLOSION_SIZE_DEFAULT 3
#define MAYHEM_BOMB_STRENGTH 5
#define HP_DEFAULT 3
#define BOMB_COUNT_DEFAULT 1
#define POWERUP_SPAWN_CHANCE_PERCENT 33