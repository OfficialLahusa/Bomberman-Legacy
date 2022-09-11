#include "GameState.hpp"

/* Returns the tile sprite of the entered ID */
sf::IntRect getTileRect(unsigned int id) {
	return sf::IntRect(sf::Vector2i(16 * id, 0), sf::Vector2i(16, 24));
}

/* Returns the animated bomb sprite by skin and frame */
sf::IntRect getBombSprite(unsigned int skin, unsigned int frame) {
	unsigned int modFrame = frame % 2;
	skin += 1; // we want the player indices to start at 1, not zero
	return sf::IntRect(sf::Vector2i(16 * 2 * skin - 16 + 16 * (modFrame - 1), 0), sf::Vector2i(16, 16));
}

/* Returns the explosion sprite by skin, type and decay state (0 = no decay, 1 = medium decay, 2 = nearly decayed) */
/* Positions: 0 = center, 1 = horizontal, 2 = right end, 3 = left end, 4 = vertical, 5 = bottom end, 6 = top end */
sf::IntRect getExplosionSprite(unsigned int skin, unsigned int type, unsigned int decay) {
	unsigned int modType = type % 6;
	return sf::IntRect(sf::Vector2i(16 * type + 16 * 3 * skin, 16 * decay), sf::Vector2i(16, 16));
}

/* Returns the animated player sprite, if given the skin id, the direction the player is facing, and the frame */
/* Directions: 0: Down, 1: Right, 2: Top, 3: Left */
/* Frames: 0: Standing, 1: Walk1, 2: Walk2 */
sf::IntRect getPlayerSprite(unsigned int skin, unsigned int direction, unsigned int frame) {
	frame = frame % 3;
	return sf::IntRect(sf::Vector2i(3 * 16 * direction + 16 * frame, 16 * skin), sf::Vector2i(16, 16));
}

/* Returns the sprite of a powerup from the texture atlas, given it's index */
sf::IntRect getPowerupSprite(unsigned int index) {
	return sf::IntRect(sf::Vector2i(16 * index, 0), sf::Vector2i(16, 16));
}

/* Returns the skin name by ID */
std::string getSkinName(unsigned int skin) {
	switch (skin) {
	case 0: return "Skullkid";
	case 1: return "Mr. Talent";
	case 2: return "Satoshi";
	case 3: return "Wrath";
	default: return "Unnamed Character";
	}
}


namespace sse
{
	GameState::GameState(GameDataRef data)
		: m_data(data)
	{

	}

	GameState::~GameState()
	{

	}

	bool GameState::Init() {

		/* Tilemap variables */
		tileset.loadFromFile("res/bomberman.png");
		tileStamp.setSize(sf::Vector2f(16, 24));
		tileStamp.setScale(TILE_SCALING, TILE_SCALING);
		tileStamp.setTexture(&tileset);

		/* Player setup */
		playerSprites.loadFromFile("res/players.png");
		players.push_back(Player(0, 0));
		players.push_back(Player(1, 1));
		players.push_back(Player(2, 2));
		players.push_back(Player(3, 3));

		aicontrollers.push_back(AIController(players[1], area, players, bombs));
		aicontrollers.push_back(AIController(players[2], area, players, bombs));
		aicontrollers.push_back(AIController(players[3], area, players, bombs));

		for (unsigned int i = 0; i < players.size(); i++) {
			players[i].shape.setTexture(&playerSprites);
		}
		

		/* Bomb variables */
		bombTex.loadFromFile("res/bombs.png");
		bombStamp.setTexture(&bombTex);
		bombStamp.setSize(sf::Vector2f(16, 16));
		bombStamp.setScale(TILE_SCALING, TILE_SCALING);
		explosionBuffer.loadFromFile("res/explosion.wav");
		explosionSound.setBuffer(explosionBuffer);
		plantBuffer.loadFromFile("res/plant.wav");
		plantSound.setBuffer(plantBuffer);
		plantSound.setVolume(70);

		/* Powerup variables */
		powerupTexture.loadFromFile("res/powerups.png");
		powerup.setTexture(&powerupTexture);
		powerup.setSize(sf::Vector2f(16, 16));
		powerup.setScale(TILE_SCALING, TILE_SCALING);
		powerupStamp.setTexture(&powerupTexture);
		powerupStamp.setSize(sf::Vector2f(16 * TILE_SCALING, 16 * TILE_SCALING));
		powerupStamp.setOrigin(8 * TILE_SCALING, 8 * TILE_SCALING);
		powerupBuffer.loadFromFile("res/powerup.wav");
		powerupSound.setBuffer(plantBuffer);
		powerupSound.setPitch(4);
		powerupSound.setVolume(50);

		/* Explosion variables */
		explosionTex.loadFromFile("res/explosions.png");
		explosionStamp.setTexture(&explosionTex);
		explosionStamp.setSize(sf::Vector2f(16, 16));
		explosionStamp.setScale(TILE_SCALING, TILE_SCALING);

		/* Collision detection and resolution variables */
		collisionShape.setSize(sf::Vector2f(16, 16));
		collisionShape.setScale(TILE_SCALING, TILE_SCALING);
		collisionShape.setFillColor(sf::Color::Red);

		/* UI Elements */
		font.loadFromFile("res/CarbonBlock.ttf");
		xText.setFillColor(sf::Color::Black);
		xText.setPosition(10, 10);
		xText.setCharacterSize(20);
		xText.setFont(font);
		yText.setFillColor(sf::Color::Black);
		yText.setPosition(10, 30);
		yText.setCharacterSize(20);
		yText.setFont(font);
		powerupCountText.setFillColor(sf::Color::Black);
		powerupCountText.setPosition(10, 30);
		powerupCountText.setCharacterSize(20);
		powerupCountText.setFont(font);
		playerText.setFillColor(sf::Color::Black);
		playerText.setPosition(10, 30);
		playerText.setCharacterSize(30);
		playerText.setFont(font);
		separatorTexture.loadFromFile("res/ui_separator.png");
		separator.setTexture(&separatorTexture);
		separator.setSize(sf::Vector2f(INFO_WIDTH * 16, 5));
		separator.setScale(TILE_SCALING, TILE_SCALING);


		/* Initialize Map */
		for (unsigned int x = 0; x < area.x(); x++) {
			for (unsigned int y = 0; y < area.y(); y++) {
				area.set(x, y, 2);
				explosions.set(x, y, 0.f);
				powerups.set(x, y, 0);
			}
		}
		for (unsigned int x = 0; x < area.x(); x++) {
			for (unsigned int y = 0; y < area.y(); y++) {
				/* If the tile is a border tile, dont modify it */
				if (x == 0 || x == area.x() - 1 || y == 0 || y == area.y() - 1) {
					continue;
				}
				else {

					/* Check whether the tile is inside the Spawn Area */
					/* Top Left */
					if (x <= SPAWN_AREA_SIZE && y <= SPAWN_AREA_SIZE) {
						area.set(x, y, 0);
						continue;
					}
					/* Top Right */
					else if (x >= area.x() - 1 - SPAWN_AREA_SIZE && y <= SPAWN_AREA_SIZE) {
						area.set(x, y, 0);
						continue;
					}
					/* Lower Left */
					else if (x <= SPAWN_AREA_SIZE && y >= area.y() - 1 - SPAWN_AREA_SIZE) {
						area.set(x, y, 0);
						continue;
					}
					/* Lower Right */
					else if (x >= area.x() - 1 - SPAWN_AREA_SIZE && y >= area.y() - 1 - SPAWN_AREA_SIZE) {
						area.set(x, y, 0);
						continue;
					}

					/* Check whether the tile should be empty */
					if (random::randomInteger<int>(0, 3) == 0 || !GENERATE_MAP) {
						area.set(x, y, 0);
					}
					else {
						/* Switch between the default mapgen tiles (With Destruction State 0) */
						switch (random::randomInteger<int>(0, 6)) {
						case 0: area.set(x, y, 2); break;
						case 1: area.set(x, y, 3); break;
						case 2: area.set(x, y, 3); break;
						case 3: area.set(x, y, 6); break;
						case 4: area.set(x, y, 6); break;
						case 5: area.set(x, y, 9); break;
						case 6: area.set(x, y, 9); break;
						}
					}

				}
			}
		}
		return true;
	}

	bool GameState::HandleInput(float dt) {
		sf::Event evnt;

		while (m_data->window.pollEvent(evnt)) {
			if (evnt.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
				m_data->window.close();
			}
		}

		/* Update Players */
		for (unsigned int i = 0; i < players.size(); i++) {
			players[i].update();
		}

		/* Input Handling */
		/* Input-Based Player Animation */
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			if (players[0].playerFrame == 0) {
				players[0].playerFrame = 1;
				players[0].playerWalkCooldown += 0.2f;
			}
			else if (players[0].playerWalkCooldown == 0) {
				if (players[0].playerFrame == 1) {
					players[0].playerFrame = 2;
					players[0].playerWalkCooldown += 0.2f;
				}
				else if (players[0].playerFrame == 2) {
					players[0].playerFrame = 1;
					players[0].playerWalkCooldown += 0.2f;
				}
			}
		}
		else if (players[0].playerWalkCooldown == 0) {
			players[0].playerFrame = 0;
			players[0].playerWalkCooldown += 0.2f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			players[0].move(sf::Vector2f(0, -MOVEMENT_SPEED * dt));
			players[0].playerDirection = 2;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			players[0].move(sf::Vector2f(0, MOVEMENT_SPEED * dt));
			players[0].playerDirection = 0;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			players[0].move(sf::Vector2f(-MOVEMENT_SPEED * dt, 0));
			players[0].playerDirection = 3;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			players[0].move(sf::Vector2f(MOVEMENT_SPEED * dt, 0));
			players[0].playerDirection = 1;
		}

		/* Update Players */
		for (unsigned int i = 0; i < players.size(); i++) {
			players[i].update();
		}

		/* Change Skin */
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab) && players[0].bombPlacementCooldown == 0) {
			unsigned int pskin = players[0].getSkin();
			if (pskin == 0) {		//Skull Kid
				pskin = 1;
				players[0].bombPlacementCooldown += 0.2f;
			}
			else if (pskin == 1) {	//Mr. Talent
				pskin = 2;
				players[0].bombPlacementCooldown += 0.2f;
			}
			else if (pskin == 2) {	//Satoshi
				pskin = 3;
				players[0].bombPlacementCooldown += 0.2f;
			}
			else if (pskin == 3) {	//Wrath
				pskin = 0;
				players[0].bombPlacementCooldown += 0.2f;
			}
			players[0].setSkin(pskin);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add) && players[0].bombPlacementCooldown == 0) {
			powerupIndex++;
			players[0].bombPlacementCooldown += 0.2f;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract) && players[0].bombPlacementCooldown == 0) {
			if (powerupIndex > 0) powerupIndex--;
			players[0].bombPlacementCooldown += 0.2f;
		}

		/* Bomb placement */
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			if (players[0].attemptBomb(bombs)) {
				plantSound.play();
			}
		}
		/* Bomb Mayhem placement */
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
			if (players[0].attemptMayhem(bombs, area)) {
				plantSound.play();
			}
		}

		return true;
	}

	bool GameState::Update(float dt) {

		/* Update AIControllers */
		for (unsigned int i = 0; i < aicontrollers.size(); i++) {
			aicontrollers[i].update(dt, plantSound);
		}

		/* Scale Powerups */
		powerupMapScale += dt * powerupScaleDifferenceFactor;
		if (powerupMapScale >= 1.f) {
			powerupScaleDifferenceFactor = -1.f;
		}
		if (powerupMapScale <= 0.f) {
			powerupScaleDifferenceFactor = 1.f;
		}

		/* Update Cooldowns for all players */
		for (unsigned int i = 0; i < players.size(); i++) {
			players[i].bombPlacementCooldown -= dt;
			if (players[i].bombPlacementCooldown <= 0.f) {
				players[i].bombPlacementCooldown = 0.f;
			}
			players[i].playerWalkCooldown -= dt;
			if (players[i].playerWalkCooldown <= 0.f) {
				players[i].playerWalkCooldown = 0.f;
			}
			players[i].hitCooldown -= dt;
			if (players[i].hitCooldown <= 0.f) {
				players[i].hitCooldown = 0.f;
			}
		}
		

		/* Collision detection */
		for (unsigned int x = 0; x < area.x(); x++) {
			for (unsigned int y = 0; y < area.y(); y++) {
				collisionShape.setPosition((x + INFO_WIDTH) * 16 * TILE_SCALING, y * 16 * TILE_SCALING + 8 * TILE_SCALING);
				if (area.at(x, y) != 0 && area.at(x, y) != 1) {
					collisionBounds = collisionShape.getGlobalBounds();

					/* Detect for every player */
					for (unsigned int i = 0; i < players.size(); i++) {
						playerBounds = players[i].shape.getGlobalBounds();
						if (collisionBounds.intersects(playerBounds)) {

							/* https://www.youtube.com/watch?v=l2iCYCLi6MU for reference */
							/* Delta vector between the centers of both axis-aligned boxes */
							sf::Vector2f translation(
								(collisionBounds.left + collisionBounds.width / 2) - (playerBounds.left + playerBounds.width / 2),
								(collisionBounds.top + collisionBounds.height / 2) - (playerBounds.top + playerBounds.height / 2)
							);

							sf::Vector2f colliderHalfSize(collisionBounds.width / 2, collisionBounds.height / 2);
							sf::Vector2f playerHalfSize(playerBounds.width / 2, playerBounds.height / 2);

							/* Negative intersection values indicate a collision */
							float intersectX = abs(translation.x) - (colliderHalfSize.x + playerHalfSize.x);
							float intersectY = abs(translation.y) - (colliderHalfSize.y + playerHalfSize.y);

							/* Move the player along the axis of least intersection */
							if (intersectX > intersectY) {
								if (translation.x > 0) {
									players[i].shape.move(intersectX, 0.f);
								}
								else {
									players[i].shape.move(-intersectX, 0.f);
								}

							}
							else {
								if (translation.y > 0) {
									players[i].shape.move(0, intersectY);
								}
								else {
									players[i].shape.move(0, -intersectY);
								}

							}
						}
					}
				}
			}
		}

		/* Damage players */
		for (unsigned int i = 0; i < players.size(); i++) {
			if (players[i].hitCooldown > 0.f) {
				continue;
			}
			else {
				if (players[i].getTilePosition().x >= 0 && players[i].getTilePosition().x < area.x() && players[i].getTilePosition().y >= 0 && players[i].getTilePosition().y < area.y()) {
					if (explosions.at(players[i].getTilePosition().x, players[i].getTilePosition().y) > 0) {
						if (players[i].lives > 0) {
							players[i].lives -= 1;
							players[i].hitCooldown += 1.f;
							std::cout << "Player " << i << " damaged.\n";
						}
						else {
							std::cout << "Player " << i << " already dead.\n";
						}
					}
				}
			}
		}

		/* Powerup pickup */
		for (unsigned int i = 0; i < players.size(); i++) {
			if (players[i].getTilePosition().x >= 0 && players[i].getTilePosition().x < area.x() && players[i].getTilePosition().y >= 0 && players[i].getTilePosition().y < area.y()) {
				unsigned int fieldPowerup = powerups.at(players[i].getTilePosition().x, players[i].getTilePosition().y);
				switch (fieldPowerup) {
				case 0: continue;
				case 1: if (players[i].lives < HP_DEFAULT) players[i].lives++; break;
				case 2: players[i].lives++; break;
				case 3: players[i].bombCount++; break;
				case 4: players[i].bombRange++; break;
				default: continue;
				}
				powerups.set(players[i].getTilePosition().x, players[i].getTilePosition().y, 0);
				powerupSound.play();
			}
		}
		return true;
	}

	bool GameState::Render(float dt) {

		xText.setString(std::to_string(players[0].getTilePosition().x));
		yText.setString(std::to_string(players[0].getTilePosition().y));

		m_data->window.clear(sf::Color(0, 144, 178, 255));	//sf::Color(40, 60, 90, 255)

		for (unsigned int i = 0; i < bombs.size(); i++) {
			if (bombs[i]->delay <= 0) {
				delete bombs[i];
				bombs.erase(bombs.begin() + i);
				explosionSound.play();
			}
			bombs[i]->Update(dt, area, explosions, powerups);
			if (bombs[i]->delay <= 0) {
				delete bombs[i];
				bombs.erase(bombs.begin() + i);
				explosionSound.play();
			}
		}

		unsigned int tileID = 0;
		/* Draw ground tiles */
		for (unsigned int y = 0; y < area.y(); y++) {
			for (unsigned int x = 0; x < area.x(); x++) {
				tileID = area.at(x, y);
				if (tileID == 0 || tileID == 1) {
					tileStamp.setTextureRect(getTileRect(tileID));
					tileStamp.setPosition((x + INFO_WIDTH) * 16 * TILE_SCALING, y * 16 * TILE_SCALING + 8 * TILE_SCALING);
					m_data->window.draw(tileStamp);
				}
			}
		}
		/* Draw Bombs */
		for (unsigned int i = 0; i < bombs.size(); i++) {
			bombStamp.setPosition((bombs[i]->posx + INFO_WIDTH) * 16 * TILE_SCALING, bombs[i]->posy * 16 * TILE_SCALING + 8 * TILE_SCALING);
			bombStamp.setTextureRect(getBombSprite(0, bombs[i]->delay / 0.5f));
			m_data->window.draw(bombStamp);
		}
		/* Draw Explosions */
		float explosionStrength;
		for (unsigned int y = 0; y < explosions.y(); y++) {
			for (unsigned int x = 0; x < explosions.x(); x++) {
				explosions.set(x, y, explosions.at(x, y) - dt);
				explosionStrength = explosions.at(x, y);
				if (explosionStrength <= 0.f) {
					explosions.set(x, y, 0.f);
				}
				else if (explosionStrength <= 0.f) {

				}
				else {

					/* Determine decay level based on explosion's strength value from the map */
					unsigned int decayState;
					if (explosionStrength >= EXPLOSION_LIFETIME * EXPLOSION_THRESHOLD_1) {
						decayState = 0;
					}
					else if (explosionStrength >= EXPLOSION_LIFETIME * EXPLOSION_THRESHOLD_2 && explosionStrength < EXPLOSION_LIFETIME * EXPLOSION_THRESHOLD_1) {
						decayState = 1;
					}
					else if (explosionStrength > 0 && explosionStrength < EXPLOSION_LIFETIME * EXPLOSION_THRESHOLD_2) {
						decayState = 2;
					}

					/* Determine the sprite to use based on the surrounding explosion levels */
					bool left = false, right = false, top = false, bottom = false;

					/* left is true, if it is out of bounds or if it is over 0.f */
					if (x - 1 < 0) {
						left = true;
					}
					else if (explosions.at(x - 1, y) > 0.f) {
						left = true;
					}

					/* right is true, if it is out of bounds or if it is over 0.f */
					if (x + 1 > explosions.x() - 1) {
						right = true;
					}
					else if (explosions.at(x + 1, y) > 0.f) {
						right = true;
					}

					/* top is true, if it is out of bounds or if it is over 0.f */
					if (y - 1 < 0) {
						top = true;
					}
					else if (explosions.at(x, y - 1) > 0.f) {
						top = true;
					}

					/* bottom is true, if it is out of bounds or if it is over 0.f */
					if (y + 1 > explosions.y() - 1) {
						bottom = true;
					}
					else if (explosions.at(x, y + 1) > 0.f) {
						bottom = true;
					}

					/* Now set the spriteID according to the calculated alignment */
					unsigned int sprite = 0;

					/* horizontal, if the top and bottom sides is false */
					if ((!top and !bottom) and left and right) sprite = 1;
					/* right end, if only the left is connected*/
					if (left and !right and !top and !bottom) sprite = 2;
					/* left end, if only the right is connected*/
					if (right and !left and !top and !bottom) sprite = 3;

					/* vertical, if the left and right sides is false */
					if ((!left and !right) and top and bottom) sprite = 4;
					/* top end, if only the bottom is connected*/
					if (bottom and !right and !top and !left) sprite = 5;
					/* bottom end, if only the top is connected*/
					if (top and !left and !bottom and !right) sprite = 6;




					/* Place and draw explosion stamp */
					explosionStamp.setPosition((x + INFO_WIDTH) * 16 * TILE_SCALING, y * 16 * TILE_SCALING + 8 * TILE_SCALING);
					explosionStamp.setTextureRect(getExplosionSprite(0, sprite, decayState));
					m_data->window.draw(explosionStamp);
				}
			}
		}
		/* Draw powerups */
		unsigned int currentPowerup;
		for (unsigned int y = 0; y < powerups.y(); y++) {
			for (unsigned int x = 0; x < powerups.x(); x++) {
				currentPowerup = powerups.at(x, y);
				if (currentPowerup > 0) {
					powerupStamp.setPosition(sf::Vector2f((x + INFO_WIDTH + 0.5f) * 16 * TILE_SCALING, (y + 0.5f) * 16 * TILE_SCALING + 8 * TILE_SCALING));
					powerupStamp.setScale((1.5f + powerupMapScale / 2.f) / 2.f, (1.5f + powerupMapScale / 2.f) / 2.f);
					unsigned int spriteID;
					switch (currentPowerup) {
					case 1: spriteID = 0; break;
					case 2: spriteID = 3; break;
					case 3: spriteID = 4; break;
					case 4: spriteID = 5; break;
					default: spriteID = 0; break;
					}
					powerupStamp.setTextureRect(getPowerupSprite(spriteID));
					m_data->window.draw(powerupStamp);
				}
			}
		}
		/* Draw overlay tiles */
		for (unsigned int y = 0; y < area.y(); y++) {
			for (unsigned int x = 0; x < area.x(); x++) {
				tileID = area.at(x, y);
				if (!(tileID == 0 || tileID == 1)) {
					tileStamp.setTextureRect(getTileRect(tileID));
					tileStamp.setPosition((x + INFO_WIDTH) * 16 * TILE_SCALING, y * 16 * TILE_SCALING);
					m_data->window.draw(tileStamp);
				}
			}

			/* Draw the player in the map row he is standing in. This way he can hide behind the next row's tiles */
			for (unsigned int i = 0; i < players.size(); i++) {
				if (players[i].lives > 0 && players[i].shape.getPosition().y <= (y + 1) * 16 * TILE_SCALING + 8 * TILE_SCALING && players[i].shape.getPosition().y >(y - 0) * 16 * TILE_SCALING + 8 * TILE_SCALING) {
					players[i].shape.setTextureRect(getPlayerSprite(players[i].getSkin(), players[i].playerDirection, players[i].playerFrame));
					m_data->window.draw(players[i].shape);
				}
			}
		}
		
		/* Draw Player list UI */
		separator.setPosition(sf::Vector2f(0, 0));
		m_data->window.draw(separator);
		for (unsigned int i = 0; i < players.size(); i++) {

			powerup.setPosition(20, i * UI_PLAYER_SEGMENT_HEIGHT + UI_PLAYER_SEGMENT_HEIGHT - 90);
			powerup.scale(0.5f, 0.5f);
			powerup.setTextureRect(getPowerupSprite(4));
			m_data->window.draw(powerup);
			powerupCountText.setPosition(sf::Vector2f(50, i * UI_PLAYER_SEGMENT_HEIGHT + UI_PLAYER_SEGMENT_HEIGHT - 90));
			powerupCountText.setString(std::to_string(players[i].bombCount));
			m_data->window.draw(powerupCountText);
			powerup.setPosition(80, i * UI_PLAYER_SEGMENT_HEIGHT + UI_PLAYER_SEGMENT_HEIGHT - 90);
			powerup.setTextureRect(getPowerupSprite(5));
			powerupCountText.setPosition(sf::Vector2f(110, i * UI_PLAYER_SEGMENT_HEIGHT + UI_PLAYER_SEGMENT_HEIGHT - 90));
			powerupCountText.setString(std::to_string(players[i].bombRange));
			m_data->window.draw(powerupCountText);
			m_data->window.draw(powerup);
			powerup.scale(2.f, 2.f);

			

			playerText.setPosition(sf::Vector2f(20, i * UI_PLAYER_SEGMENT_HEIGHT + UI_PLAYER_SEGMENT_HEIGHT-75));
			playerText.setString("Player " + std::to_string(i) + ": " + getSkinName(players[i].getSkin())); // + " (" + std::to_string(players[i].bombsCurrent) + ")"
			m_data->window.draw(playerText);

			/* How many sprites must be drawn */
			unsigned int lives_draw_count;
			if (players[i].lives > HP_DEFAULT) {
				lives_draw_count = players[i].lives;
			}
			else {
				lives_draw_count = HP_DEFAULT;
			}

			for (unsigned int j = 0; j < lives_draw_count; j++) {
				powerup.setPosition(20 + (16 + 2) * j * TILE_SCALING, i * UI_PLAYER_SEGMENT_HEIGHT + UI_PLAYER_SEGMENT_HEIGHT-45);
				if (j + 1 > players[i].lives) {
					powerup.setTextureRect(getPowerupSprite(1));
				}
				else if (players[i].hitCooldown > 0.75f) {
					powerup.setTextureRect(getPowerupSprite(2));
				}
				else if (j + 1 > HP_DEFAULT + 3) {
					powerup.setTextureRect(getPowerupSprite(-1));
				}
				else if (j + 1 == HP_DEFAULT + 3 && players[i].lives > HP_DEFAULT + 3) {
					powerup.setTextureRect(getPowerupSprite(6));
				}
				else if (j + 1 > HP_DEFAULT) {
					powerup.setTextureRect(getPowerupSprite(3));
				}
				else {
					powerup.setTextureRect(getPowerupSprite(0));
				}
				m_data->window.draw(powerup);
			}
			separator.setPosition(sf::Vector2f(0, i * UI_PLAYER_SEGMENT_HEIGHT + UI_PLAYER_SEGMENT_HEIGHT));
			m_data->window.draw(separator);
		}

		
		//m_data->window.draw(xText);
		//m_data->window.draw(yText);

		m_data->window.display();
		return true;
	}
}