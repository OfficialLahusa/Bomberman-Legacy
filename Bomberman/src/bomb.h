#pragma once

#include "../SimpleSfmlEngine/engine.hpp"
#include "def.h"

#include <vector>
#include <iostream>

class Player;

class Bomb {

private:

	/* This function calculates, how much damage to which tile results in which new tile */
	unsigned int destroyBlock(unsigned int oldTile) {
		switch (oldTile) {
		case 0: oldTile = 1; break;			//Normal Ground -> Dirty Ground
		case 1: break;						//Dirty Ground remains the same
		case 2: break;						//Iron Crate remains the same
		case 3: oldTile = 4; break;			//Wooden Crate State 1 -> Wooden Crate State 2
		case 4: oldTile = 5; break;			//Wooden Crate State 2 -> Wooden Crate State 3
		case 5: oldTile = 0; break;			//Wooden Crate State 3 -> Normal Ground
		case 6: oldTile = 7; break;			//Bush State 1 -> Bush State 2
		case 7: oldTile = 8; break;			//Bush State 2 -> Bush State 3
		case 8: oldTile = 0; break;			//Bush State 3 -> Normal Ground
		case 9: oldTile = 10; break;		//Log State 1 -> Log State 2
		case 10: oldTile = 11; break;		//Log State 2 -> Log State 3
		case 11: oldTile = 0; break;		//Log State 3 -> Normal Ground
		default: oldTile = 0; break;		//Undefined -> Normal Ground
		}
		return oldTile;
	}

public:
	float delay;
	unsigned int* bombCounter;
	unsigned int strength;
	unsigned int posx, posy;

	Bomb(unsigned int* bombCounter, unsigned int bombStrength, unsigned int x, unsigned int y)
		: bombCounter(bombCounter)
		, delay(5.f)
		, strength(bombStrength)
		, posx(x)
		, posy(y)
	{
		if (bombCounter != nullptr) {
			*bombCounter += 1;
		}
		//std::cout << "CONSTRUCTED: Bomb by player " << owner << " at " << posx << ", " << posy << "\n";
	}

	~Bomb() {
		if (bombCounter != nullptr) {
			*bombCounter -= 1;
		}
		//std::cout << "DECONSTRUCTED: Bomb by player " << owner << " at " << posx << ", " << posy << "\n";
	}

	void Update(float deltaTime, sse::Matrix<unsigned int, TILES_X, TILES_Y>& map, sse::Matrix<float, TILES_X, TILES_Y > & explosions, sse::Matrix<unsigned int, TILES_X, TILES_Y > & powerups) {
		delay -= deltaTime;
		if (delay <= 0) {
			detonate(map, explosions, powerups);
		}
	}

	void detonate(sse::Matrix<unsigned int, TILES_X, TILES_Y>& map, sse::Matrix<float, TILES_X, TILES_Y>& explosions, sse::Matrix<unsigned int, TILES_X, TILES_Y > & powerups) {
		/*map.SetVal(posx, posy - 1, destroyBlock(map.GetVal(posx, posy - 1), strength));
		map.SetVal(posx - 1, posy, destroyBlock(map.GetVal(posx - 1, posy), strength));
		map.SetVal(posx, posy + 1, destroyBlock(map.GetVal(posx, posy + 1), strength));
		map.SetVal(posx + 1, posy, destroyBlock(map.GetVal(posx + 1, posy), strength));
		map.SetVal(posx, posy, destroyBlock(map.GetVal(posx, posy), strength));*/

		/* Process center block */
		map.set(posx, posy, destroyBlock(map.at(posx, posy)));

		/* Execute this only, if the center block was destroyed */
		if (map.at(posx, posy) == 0 || map.at(posx, posy) == 1) {
			/* Place center block's explosion */
			explosions.set(posx, posy, EXPLOSION_LIFETIME);
		}

		float tempstrength;
		int xoff, yoff;
		unsigned int tile;
		bool hitEnd;

		/* Continue upwards from center */
		tempstrength = strength;
		xoff = 0;
		yoff = 0;
		hitEnd = false;
		while (!hitEnd) {

			/* go up one */
			yoff--;

			/* Break, if the coordinates are out of bound, or if the bomb strength is zero or less */
			if (posx + xoff < 0 || posy + yoff < 0 || posx + xoff > map.x() - 1 || posy + yoff > map.y() - 1 || tempstrength <= 0) {
				hitEnd = true;
			}
			else {
				tile = map.at(posx + xoff, posy + yoff);
				/* Process tile */
				map.set(posx + xoff, posy + yoff, destroyBlock(tile));
				/* If the tile was solid, break */
				if (tile != 0 && tile != 1) {
					hitEnd = true;
					if (destroyBlock(tile) == 0 || destroyBlock(tile) == 1) {
						/* Spawn powerup */
						if (sse::random::randomInteger<int>(1, 100) <= POWERUP_SPAWN_CHANCE_PERCENT) {
							powerups.set(posx + xoff, posy + yoff, sse::random::randomInteger<int>(1, 4));
						}
					}
				}
				else {
					explosions.set(posx + xoff, posy + yoff, EXPLOSION_LIFETIME);
					tempstrength--;
				}
			}
		}

		/* Continue downwards from center */
		tempstrength = strength;
		xoff = 0;
		yoff = 0;
		hitEnd = false;
		while (!hitEnd) {

			/* go down one */
			yoff++;

			/* Break, if the coordinates are out of bound, or if the bomb strength is zero or less */
			if (posx + xoff < 0 || posy + yoff < 0 || posx + xoff > map.x() - 1 || posy + yoff > map.y() - 1 || tempstrength <= 0) {
				hitEnd = true;
			}
			else {
				tile = map.at(posx + xoff, posy + yoff);
				/* Process tile */
				map.set(posx + xoff, posy + yoff, destroyBlock(tile));
				/* If the tile was solid, break */
				if (tile != 0 && tile != 1) {
					hitEnd = true;
					if (destroyBlock(tile) == 0 || destroyBlock(tile) == 1) {
						/* Spawn powerup */
						if (sse::random::randomInteger<int>(1, 100) <= POWERUP_SPAWN_CHANCE_PERCENT) {
							powerups.set(posx + xoff, posy + yoff, sse::random::randomInteger<int>(1, 4));
						}
					}
				}
				else {
					explosions.set(posx + xoff, posy + yoff, EXPLOSION_LIFETIME);
					tempstrength--;
				}
			}
		}

		/* Continue left from center */
		tempstrength = strength;
		xoff = 0;
		yoff = 0;
		hitEnd = false;
		while (!hitEnd) {

			/* go up one */
			xoff--;

			/* Break, if the coordinates are out of bound, or if the bomb strength is zero or less */
			if (posx + xoff < 0 || posy + yoff < 0 || posx + xoff > map.x() - 1 || posy + yoff > map.y() - 1 || tempstrength <= 0) {
				hitEnd = true;
			}
			else {
				tile = map.at(posx + xoff, posy + yoff);
				/* Process tile */
				map.set(posx + xoff, posy + yoff, destroyBlock(tile));
				/* If the tile was solid, break */
				if (tile != 0 && tile != 1) {
					hitEnd = true;
					if (destroyBlock(tile) == 0 || destroyBlock(tile) == 1) {
						/* Spawn powerup */
						if (sse::random::randomInteger<int>(1, 100) <= POWERUP_SPAWN_CHANCE_PERCENT) {
							powerups.set(posx + xoff, posy + yoff, sse::random::randomInteger<int>(1, 4));
						}
					}
				}
				else {
					explosions.set(posx + xoff, posy + yoff, EXPLOSION_LIFETIME);
					tempstrength--;
				}
			}
		}

		/* Continue right from center */
		tempstrength = strength;
		xoff = 0;
		yoff = 0;
		hitEnd = false;
		while (!hitEnd) {

			/* go down one */
			xoff++;

			/* Break, if the coordinates are out of bound, or if the bomb strength is zero or less */
			if (posx + xoff < 0 || posy + yoff < 0 || posx + xoff > map.x() - 1 || posy + yoff > map.y() - 1 || tempstrength <= 0) {
				hitEnd = true;
			}
			else {
				tile = map.at(posx + xoff, posy + yoff);
				/* Process tile */
				map.set(posx + xoff, posy + yoff, destroyBlock(tile));
				/* If the tile was solid, break */
				if (tile != 0 && tile != 1) {
					hitEnd = true;
					if (destroyBlock(tile) == 0 || destroyBlock(tile) == 1) {
						/* Spawn powerup */
						if (sse::random::randomInteger<int>(1, 100) <= POWERUP_SPAWN_CHANCE_PERCENT) {
							powerups.set(posx + xoff, posy + yoff, sse::random::randomInteger<int>(1, 4));
						}
					}
				}
				else {
					explosions.set(posx + xoff, posy + yoff, EXPLOSION_LIFETIME);
					tempstrength--;
				}
			}
		}

		
		
		delay = 0.f;

		//std::cout << "DETONATED (Strength: " << strength << "): Bomb by player " << owner << " at " << posx << ", " << posy << "\n";
	}

};