#pragma once
#include "../../SimpleSfmlEngine/engine.hpp"

#include "../def.h"
#include "../bomb.h"
#include "../player.h"
#include "../AIController.h"

#include <iostream>

/* Returns the tile sprite of the entered ID */
sf::IntRect getTileRect(unsigned int id);

/* Returns the animated bomb sprite by skin and frame */
sf::IntRect getBombSprite(unsigned int skin, unsigned int frame);

/* Returns the explosion sprite by skin, type and decay state (0 = no decay, 1 = medium decay, 2 = nearly decayed) */
/* Positions: 0 = center, 1 = horizontal, 2 = right end, 3 = left end, 4 = vertical, 5 = bottom end, 6 = top end */
sf::IntRect getExplosionSprite(unsigned int skin, unsigned int type, unsigned int decay);

/* Returns the animated player sprite, if given the skin id, the direction the player is facing, and the frame */
/* Directions: 0: Down, 1: Right, 2: Top, 3: Left */
/* Frames: 0: Standing, 1: Walk1, 2: Walk2 */
sf::IntRect getPlayerSprite(unsigned int skin, unsigned int direction, unsigned int frame);

/* Returns the sprite of a powerup from the texture atlas, given it's index */
sf::IntRect getPowerupSprite(unsigned int index);

/* Returns the skin name by ID */
std::string getSkinName(unsigned int skin);


namespace sse
{
	class GameState : public State
	{
	public:
		explicit GameState(GameDataRef data);
		~GameState();

		bool Init() override;

		bool HandleInput(float dt) override;
		bool Update(float dt) override;
		bool Render(float dt) override;

	private:

		GameDataRef m_data;

		/* Tilemap variables */
		sf::Texture tileset;
		sf::RectangleShape tileStamp;

		/* Player setup */
		sf::Texture playerSprites;
		std::vector<Player> players;
		std::vector<AIController> aicontrollers;

		/* Bomb variables */
		std::vector<Bomb*> bombs;
		sf::Texture bombTex;
		sf::RectangleShape bombStamp;
		sf::Sound explosionSound;
		sf::SoundBuffer explosionBuffer;
		sf::Sound plantSound;
		sf::SoundBuffer plantBuffer;

		/* Powerup variables */
		unsigned int powerupIndex = 0;
		float powerupMapScale = 0;
		float powerupScaleDifferenceFactor = 1.f;
		sf::RectangleShape powerup; //For the UI
		sf::Texture powerupTexture;
		sf::RectangleShape powerupStamp; //For the Map
		sf::Sound powerupSound;
		sf::SoundBuffer powerupBuffer;

		/* Explosion variables */
		sf::Texture explosionTex;
		sf::RectangleShape explosionStamp;

		/* Collision detection and resolution variables */
		sf::RectangleShape collisionShape;
		sf::FloatRect collisionBounds, playerBounds;

		/* UI Elements */
		sf::Font font;
		sf::Text xText;
		sf::Text yText;
		sf::Text powerupCountText;
		sf::Text playerText;
		sf::Texture separatorTexture;
		sf::RectangleShape separator;

		/* Map Container, where each tile is represented by an unsigned int representing it's id in the tile atlas */
		Matrix<unsigned int, TILES_X, TILES_Y> area;
		/* Explosion strength map */
		Matrix<float, TILES_X, TILES_Y> explosions;
		/* Powerup map */
		Matrix<unsigned int, TILES_X, TILES_Y> powerups;

	};
}