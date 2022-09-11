#include "AIController.h"

sf::Vector2f normalize(const sf::Vector2f& source)
{
	float length = sqrt((source.x * source.x) + (source.y * source.y));
	if (length != 0)
		return sf::Vector2f(source.x / length, source.y / length);
	else
		return source;
}

AIController::AIController(Player& player, sse::Matrix<unsigned int, TILES_X, TILES_Y>& area, std::vector<Player>& players, std::vector<Bomb*>& bombs)
	: m_player(player)
	, m_area(area)
	, m_players(players)
	, m_bombs(bombs)
{

}

AIController::~AIController() {
	directionChangeTime = sse::random::randomReal<float>(0.f, 1.f);
}

void AIController::update(float dt, sf::Sound& placementSound) {
	if (m_active) {
		
		// TODO: A* pathfinding, individual agression-based enemy ranking, Switching between prioritized survival, fighting players and powerup collection
		// LIFEHACK: For the A* pathfinding, (possibly) use very high step values for the obstacles instead of excluding them, as they can be broken and turned into a pathway.
		// LIFEHACK: Use "possibly dangerous" marking for tiles in explosion range of bombs and assign them higher step values

		if (m_player.lives <= 0) {
			return;
		}

		directionChangeTime += dt;
		while (directionChangeTime > 1.f) {
			directionChangeTime -= 1.f;
			movementDirection = sf::Vector2f(sse::random::randomInteger<int>(-1, 1), sse::random::randomInteger<int>(-1, 1));
		}
		sf::Vector2f deltaPosition(m_player.shape.getPosition() - m_players[0].shape.getPosition());
		deltaPosition = normalize(deltaPosition);
		deltaPosition = movementDirection * dt * MOVEMENT_SPEED;

		// TODO: Insert proper bomb placement logic
		if (m_player.bombsCurrent < m_player.bombCount && m_player.bombPlacementCooldown == 0.f && sse::random::randomInteger<int>(0, 200) == 200) {
			m_player.attemptBomb(m_bombs);
			placementSound.play();
		}
		
		// TODO: Insert proper logic to determine the facing direction of the AI Player
		m_player.move(deltaPosition);
	}
}

bool AIController::isActive() const {
	return m_active;
}

void AIController::setActive(bool active) {
	m_active = active;
}