#pragma once
#include <SFML/Graphics.hpp>
#include "def.h"
#include "bomb.h"

class Player {
private:
	unsigned int m_skin;
	unsigned int m_x, m_y;
	unsigned int m_id;

public:
	unsigned int bombRange = EXPLOSION_SIZE_DEFAULT;
	unsigned int bombCount = BOMB_COUNT_DEFAULT;
	unsigned int bombsCurrent = 0;
	unsigned int playerFrame = 0;
	unsigned int playerDirection = 0;
	float playerWalkCooldown = 0.f;
	float bombPlacementCooldown = 0.f;
	float hitCooldown = 0.f;
	unsigned int lives = HP_DEFAULT;
	sf::RectangleShape shape;

	Player(unsigned int newID = 0, unsigned int newSkin = 0) {

		m_id = newID;
		m_skin = newSkin;
		shape.setSize(sf::Vector2f(16 * TILE_SCALING, 16 * TILE_SCALING));

		/* Spawn Player in one of the map's corners */
		switch (m_id % 4) {
		case 0: shape.setPosition((INFO_WIDTH * 16 * TILE_SCALING) + 20 * TILE_SCALING, 16 * TILE_SCALING + 12 * TILE_SCALING); break;	//Top Left
		case 1: shape.setPosition((INFO_WIDTH * 16 * TILE_SCALING) + (16 * TILES_X + 4) * TILE_SCALING - (32 * TILE_SCALING), 16 * TILE_SCALING + 12 * TILE_SCALING); break; //Top Right
		case 2: shape.setPosition((INFO_WIDTH * 16 * TILE_SCALING) + 20 * TILE_SCALING, (16 * TILES_Y) * TILE_SCALING - (32 * TILE_SCALING) + 12 * TILE_SCALING); break; //Bottom Left
		case 3: shape.setPosition((INFO_WIDTH * 16 * TILE_SCALING) + (16 * TILES_X + 4) * TILE_SCALING - (32 * TILE_SCALING), (16 * TILES_Y) * TILE_SCALING - (32 * TILE_SCALING) + 12 * TILE_SCALING); break; //Bottom Right
		default: shape.setPosition((INFO_WIDTH * 16 * TILE_SCALING) + 20 * TILE_SCALING, 16 * TILE_SCALING + 12 * TILE_SCALING); break; //Top Left
		}
		
	}

	void update() {
		m_x = ((shape.getPosition().x - 16 * INFO_WIDTH * TILE_SCALING) + 6 * TILE_SCALING) / (16 * TILE_SCALING);
		m_y = (shape.getPosition().y + 2 * TILE_SCALING) / (16 * TILE_SCALING);
	}

	void move(sf::Vector2f movement) {
		shape.move(movement);
	}

	bool attemptBomb(std::vector<Bomb*>& bombs) {
		if (bombPlacementCooldown == 0.f && bombsCurrent < bombCount) {
			bombs.push_back( new Bomb(&bombsCurrent, bombRange, getTilePosition().x, getTilePosition().y) );
			bombPlacementCooldown += 0.5f;
			return true;
		}
		return false;
	}

	bool attemptMayhem(std::vector<Bomb*>& bombs, sse::Matrix<unsigned int, TILES_X, TILES_Y>& area) {
		if (bombPlacementCooldown == 0.f) {
			for (unsigned int x = 0; x < area.x(); x++) {
				for (unsigned int y = 0; y < area.y(); y++) {
					if (area.at(x, y) == 0 || area.at(x, y) == 1) {
						/* Don't count placed bombs to the bomb counter */
						bombs.push_back(new Bomb(nullptr, MAYHEM_BOMB_STRENGTH, x, y));
					}
				}
			}
			bombPlacementCooldown += 0.5f;
			return true;
		}
		return false;
	}

	bool isDead() const {
		if (lives <= 0) {
			return true;
		}
		else {
			return false;
		}
	}

	sf::Vector2i getTilePosition() const {
		return sf::Vector2i(m_x, m_y);
	}

	unsigned int getSkin() const {
		return m_skin;
	}

	unsigned int getID() const {
		return m_id;
	}

	void setSkin(unsigned int newSkin = 0) {
		m_skin = newSkin;
	}

	unsigned int setTilePosition(unsigned int tx, unsigned int ty) {
		m_x = tx;
		m_y = ty;
		shape.setPosition((tx + INFO_WIDTH) * 16 * TILE_SCALING, ty * 16 * TILE_SCALING);
	}

};