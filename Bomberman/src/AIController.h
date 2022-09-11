#pragma once
#include "player.h"
#include "../SimpleSfmlEngine/engine.hpp"

sf::Vector2f normalize(const sf::Vector2f& source);

class Bomb;

class AIController {
private:
	Player & m_player;
	bool m_active = true;
	sse::Matrix<unsigned int, TILES_X, TILES_Y>& m_area;
	std::vector<Player>& m_players;
	std::vector<Bomb*>& m_bombs;
	float directionChangeTime = 0.f;
	sf::Vector2f movementDirection;

public:
	AIController(Player& player, sse::Matrix<unsigned int, TILES_X, TILES_Y>& area, std::vector<Player>& players, std::vector<Bomb*>& bombs);
	~AIController();

	void update(float dt, sf::Sound& placementSound);

	bool isActive() const;

	void setActive(bool active);
};