#include <iostream>
#include <random>
#include <time.h>
#include <vector>
#include <chrono>
#include <thread>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#define SERVER_TICKRATE 60
#define SERVER_PORT 5555

#define PING_PACKET_ID 0
#define PING_RESPONSE_PACKET_ID 1
#define PLAYER_JOIN_PACKET_ID 2
#define PLAYER_MOVE_PACKET_ID 3
#define PLAYER_JOIN_RESPONSE_PACKET_ID 4
#define PLAYER_STATUS_PACKET_ID 5
#define PLAYER_LEAVE_PACKET_ID 6

#define DEFAULT_USE_CLIENT_PREDICTION false
#define DEFAULT_USE_ENTITY_INTERPOLATION false

#define PING_ATTEMPTS 1
#define HIDE_PINGING false

#define RELEASE_LOGGING false

#if ! defined(_DEBUG) && RELEASE_LOGGING == false
#define DEBUGONLY(x) (void)0
#else
#define DEBUGONLY(x) x
#endif

class PlayerEntity {
public:
	PlayerEntity() {

	}
	PlayerEntity(sf::Color color, sf::Vector2f position) {
		this->color = color;
		this->position = position;
	}
	sf::Color color;
	sf::Vector2f position;
	float timestamp;
	sf::Vector2f previousPosition;
	float previousTimestamp;
};
class PlayerClient {
public:
	PlayerClient() {

	}
	PlayerEntity entity;
	sf::IpAddress endpoint;
	short endpointPort;
	int uniqueAccessToken;
};
class BufferedPacket {
public:
	BufferedPacket() {

	}
	BufferedPacket(sf::Packet packet, sf::IpAddress sender, short port) {
		this->packet = packet;
		this->sender = sender;
		this->port = port;
	}
	sf::Packet packet;
	sf::IpAddress sender;
	short port;
};
float lerp(float a, float b, float f)
{
	return (a * (1.0 - f)) + (b * f);
}

int main(unsigned int argc, char** argv) {
	
	const short port = SERVER_PORT;
	char input;

	std::cout << "Start as server (y/n)\n";
	std::cin >> input;

	sf::UdpSocket socket;

	// Launch as Server
	if (input == 'y') {

		sf::Clock serverClock;
		
		std::vector<BufferedPacket> packetBuffer;
		std::vector<PlayerClient> clients;

		socket.bind(port);
		socket.setBlocking(false);

		std::cout << "Server bound to port " << port << "\n";

		sf::Clock deltaClock;
		float deltaTime;
		float timeWaitedUntilNextTick = 0.f;

		while (true) {
			// Update delta time and increase tick wait time
			deltaTime = deltaClock.restart().asSeconds();
			timeWaitedUntilNextTick += deltaTime;

			// Fetch incoming packets and buffer them
			sf::Packet packet;
			sf::IpAddress sender;
			unsigned short senderPort;
			bool sendStatusPacket = false;
			bool readMore = true;

			do {
				if (socket.receive(packet, sender, senderPort) == sf::Socket::Error)
					return EXIT_FAILURE;

				short header;

				if (packet.getDataSize() > 0) {

					packet >> header;

					switch (header) {
					// Unhandled Packet
					default: {
						DEBUGONLY(std::cout << "Unknown packet (" << packet.getDataSize() << " Bytes) received from client " << sender << " with header " << header << std::endl);
						break;
					}
					// Respond to ping packets immediately
					case PING_PACKET_ID: {
						if (!HIDE_PINGING) {
							DEBUGONLY(std::cout << "Ping packet (" << packet.getDataSize() << " Bytes) received from client " << sender << std::endl);
						}
						sf::Packet pingResponsePacket;
						pingResponsePacket << (short)PING_RESPONSE_PACKET_ID << (float)serverClock.getElapsedTime().asMilliseconds();
						socket.send(pingResponsePacket, sender, senderPort);
						if (!HIDE_PINGING) {
							DEBUGONLY(std::cout << "Ping response packet (" << pingResponsePacket.getDataSize() << " Bytes) sent to client " << sender << std::endl);
						}
						break;
					}
					case PLAYER_JOIN_PACKET_ID: {
						sf::Packet bufferPacket;
						bufferPacket.append(packet.getData(), packet.getDataSize());
						packetBuffer.push_back(BufferedPacket(bufferPacket, sender, senderPort));
						break;
					}
					case PLAYER_LEAVE_PACKET_ID: {
						sf::Packet bufferPacket;
						bufferPacket.append(packet.getData(), packet.getDataSize());
						packetBuffer.push_back(BufferedPacket(bufferPacket, sender, senderPort));
						break;
					}
					case PLAYER_MOVE_PACKET_ID: {
						sf::Packet bufferPacket;
						bufferPacket.append(packet.getData(), packet.getDataSize());
						packetBuffer.push_back(BufferedPacket(bufferPacket, sender, senderPort));
						break;
					}
					}
				}
				else {
					readMore = false;
				}
			} while (readMore);

			// Handle packets 
			if (timeWaitedUntilNextTick >= 1.f / SERVER_TICKRATE) {

				//std::cout << packetBuffer.size() << " packets in handle queue\n";

				for (unsigned int packetIndex = 0; packetIndex < packetBuffer.size(); packetIndex++) {

					short header;

					packetBuffer[packetIndex].packet >> header;

					switch (header) {
						// Unhandled Packet
					default: {
						DEBUGONLY(std::cout << "Unknown packet (" << packetBuffer[packetIndex].packet.getDataSize() << " Bytes) in packet buffer from client " << packetBuffer[packetIndex].sender << " with header " << header << std::endl);
						break;
					}
					case PLAYER_JOIN_PACKET_ID: {
						int r, g, b;
						float x, y;
						packetBuffer[packetIndex].packet >> r >> g >> b >> x >> y;
						DEBUGONLY(std::cout << "Player join packet (" << packetBuffer[packetIndex].packet.getDataSize() << " Bytes) handled from client " << packetBuffer[packetIndex].sender << " with color " << r << ", " << g << ", " << b << " at position " << x << ", " << y << std::endl);
						PlayerClient client;
						client.endpoint = packetBuffer[packetIndex].sender;
						client.endpointPort = packetBuffer[packetIndex].port;
						client.entity = PlayerEntity(sf::Color(r, g, b), sf::Vector2f(x, y));
						srand(time(NULL));
						client.uniqueAccessToken = rand();
						clients.push_back(client);
						sf::Packet playerJoinResponsePacket;
						playerJoinResponsePacket << (short)PLAYER_JOIN_RESPONSE_PACKET_ID << (float)serverClock.getElapsedTime().asMilliseconds() << clients.size() << client.uniqueAccessToken;
						for (unsigned int i = 0; i < clients.size(); i++) {
							playerJoinResponsePacket << clients[i].entity.color.r << clients[i].entity.color.g << clients[i].entity.color.b << clients[i].entity.position.x << clients[i].entity.position.y;
						}
						socket.send(playerJoinResponsePacket, packetBuffer[packetIndex].sender, packetBuffer[packetIndex].port);
						DEBUGONLY(std::cout << "Player join packet (" << playerJoinResponsePacket.getDataSize() << " Bytes) sent to client " << client.endpoint << " [Unique Access Token: " << client.uniqueAccessToken << "] with color " << r << ", " << g << ", " << b << " at position " << x << ", " << y << std::endl);

						// Send player join packet to all clients except the new one
						sf::Packet playerJoinPacket;
						playerJoinPacket << (short)PLAYER_JOIN_PACKET_ID << (float)serverClock.getElapsedTime().asMilliseconds() << r << g << b << x << y;
						for (unsigned int i = 0; i < clients.size() - 1; i++) {
							socket.send(playerJoinPacket, clients[i].endpoint, clients[i].endpointPort);
							DEBUGONLY(std::cout << "Player join packet (" << playerJoinPacket.getDataSize() << " Bytes) sent to client " << clients[i].endpoint << " of client " << client.endpoint << " with color " << r << ", " << g << ", " << b << " at position " << x << ", " << y << std::endl);
						}
						break;
					}
					case PLAYER_LEAVE_PACKET_ID: {
						int accessToken;
						packetBuffer[packetIndex].packet >> accessToken;
						DEBUGONLY(std::cout << "Player leave packet (" << packetBuffer[packetIndex].packet.getDataSize() << " Bytes) handled from client " << packetBuffer[packetIndex].sender << std::endl);

						int index;
						for (int i = 0; i < clients.size(); i++) {
							if (clients[i].uniqueAccessToken == accessToken) {
								index = i;
							}
						}

						// Send player leave packet to all clients
						sf::Packet playerLeavePacket;
						playerLeavePacket << (short)PLAYER_LEAVE_PACKET_ID << index;
						for (unsigned int i = 0; i < clients.size(); i++) {
							if (i == index) {
								continue;
							}
							socket.send(playerLeavePacket, clients[i].endpoint, clients[i].endpointPort);
							DEBUGONLY(std::cout << "Player leave packet (" << playerLeavePacket.getDataSize() << " Bytes) sent to client " << clients[i].endpoint << " of client " << clients[index].endpoint << std::endl);
						}

						clients.erase(clients.begin() + index);
						break;
					}
					case PLAYER_MOVE_PACKET_ID: {
						float x, y;
						int suppliedAccessToken;
						packetBuffer[packetIndex].packet >> suppliedAccessToken >> x >> y;
						DEBUGONLY(std::cout << "Player move packet (" << packetBuffer[packetIndex].packet.getDataSize() << " Bytes) handled from client " << packetBuffer[packetIndex].sender << " (x: " << x << ", y: " << y << ")" << std::endl);
						for (unsigned int i = 0; i < clients.size(); i++) {
							if (clients[i].uniqueAccessToken == suppliedAccessToken) {
								clients[i].entity.position.x = clients[i].entity.position.x + x;
								clients[i].entity.position.y = clients[i].entity.position.y + y;
							}
						}
						sendStatusPacket = true;
						break;
					}
					}
				}

				// Clear packets
				packetBuffer.clear();

				// Reset tick wait time
				timeWaitedUntilNextTick = 0;
			}
			

			if (sendStatusPacket) {
				sf::Packet playerStatusPacket;
				playerStatusPacket << (short)PLAYER_STATUS_PACKET_ID << (float)serverClock.getElapsedTime().asMilliseconds() << clients.size();
				for (unsigned int i = 0; i < clients.size(); i++) {
					playerStatusPacket << clients[i].entity.position.x << clients[i].entity.position.y;
				}
				for (unsigned int i = 0; i < clients.size(); i++) {
					socket.send(playerStatusPacket, clients[i].endpoint, clients[i].endpointPort);
				}
				DEBUGONLY(std::cout << "Sent status packet to " << clients.size() << " clients\n");
			}

			/*deltaTime = deltaClock.restart().asSeconds();
			if (deltaTime < (1.f / (float)SERVER_TICKRATE)) {
				std::this_thread::sleep_for(std::chrono::microseconds((long)(1000000*((1.f / (float)SERVER_TICKRATE) - deltaTime))));
			}*/
		}
	}
	// Launch as Client
	else {

		sf::Clock clientClock;

		sf::IpAddress server;
		do
		{
			std::cout << "Type the address or name of the server to connect to: ";
			std::cin >> server;
		} while (server == sf::IpAddress::None);

		std::vector<PlayerEntity> players;
		int indexOfLocalPlayer;
		int uniqueAccessToken;

		// Initialize Font
		sf::Font fontArial;
		if (!fontArial.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
			std::cerr << "[Error] Could not load font \"arial.ttf\"" << std::endl;
		}

		// Initialize Debug Text
		sf::Text entityInterpolationText("Entity Interpolation [N]", fontArial, 15);
		sf::Text clientPredictionText("Client Prediction [C]", fontArial, 15);
		entityInterpolationText.setPosition(5, 5);
		clientPredictionText.setPosition(5, 25);
		bool useEntityInterpolation = DEFAULT_USE_ENTITY_INTERPOLATION, useClientPrediction = DEFAULT_USE_CLIENT_PREDICTION;
		float toggleCooldown = 0.f;

		// Initialize Player Stamp
		sf::CircleShape playerStamp;
		playerStamp.setRadius(20);
		playerStamp.setOrigin(20, 20);

		// Initialize Local Player Entity
		srand(time(NULL));
		PlayerEntity player;
		sf::Color playerColor(rand() % 256, rand() % 256, rand() % 256);
		player.color = playerColor;
		player.position = sf::Vector2f(250, 250);

		float totalPing = 0.f;
		float serverTime;
		// Send ping packet to server
		sf::Packet pingPacket;
		pingPacket << (short)PING_PACKET_ID;
		socket.send(pingPacket, server, port);
		if (!HIDE_PINGING) {
			DEBUGONLY(std::cout << "Ping packet (" << pingPacket.getDataSize() << " Bytes) sent to server " << server << std::endl);
		}
		sf::Clock pingClock;

		// Fetch ping response packet
		sf::Packet pingResponsePacket;
		sf::IpAddress pingResponseSender;
		unsigned short pingResponsePort;
		short pingResponsePacketHeader;
		do {
			socket.receive(pingResponsePacket, pingResponseSender, pingResponsePort);
			pingResponsePacket >> pingResponsePacketHeader;
		} while (pingResponsePacketHeader != (short)PING_RESPONSE_PACKET_ID);
		if (!HIDE_PINGING) {
			DEBUGONLY(std::cout << "Ping response packet (" << pingResponsePacket.getDataSize() << " Bytes) received from server " << pingResponseSender << std::endl);
		}
		pingResponsePacket >> serverTime;

		// Calculate ping
		float ping = pingClock.getElapsedTime().asMilliseconds();
		if (!HIDE_PINGING) {
			std::cout << "Ping: " << std::roundf(ping) << "ms" << std::endl;
		}
		float clientTime = clientClock.getElapsedTime().asMilliseconds();
		float timestampCorrection = serverTime - (clientTime - ping / 2.f);
		std::cout << "Timestamp correction: " << timestampCorrection << "ms (server: " << serverTime << "ms, client: " << clientTime << "ms)\n";

		// Prepare and send Player Join Packet
		sf::Packet playerJoinPacket;
		playerJoinPacket << (short)PLAYER_JOIN_PACKET_ID << (int)playerColor.r << (int)playerColor.g << (int)playerColor.b << player.position.x << player.position.y;
		socket.send(playerJoinPacket, server, port);
		DEBUGONLY(std::cout << "Player join packet sent to server at " << server << " (Size: " << playerJoinPacket.getDataSize() << ")" << std::endl);

		// Fetch Player Join Response Packet
		sf::Packet playerJoinResponsePacket;
		sf::IpAddress responseSender;
		unsigned short receivedPort;
		if (socket.receive(playerJoinResponsePacket, responseSender, receivedPort) != sf::Socket::Done) {
			std::cerr << "[Error] Didn't receive join packet" << std::endl;
			std::cin.get();
			return EXIT_FAILURE;
		}

		// Check for IP mismatch
		if (responseSender.getPublicAddress() != server.getPublicAddress()) {
			std::cerr << "[Error] Join response packet from wrong IP: " << responseSender << ":" << receivedPort << std::endl;
			std::cin.get();
			return EXIT_FAILURE;
		}
		else {
			short playerJoinResponseHeader;
			float timestamp;
			int vectorSize;
			playerJoinResponsePacket >> playerJoinResponseHeader >> timestamp >> vectorSize >> uniqueAccessToken;
			indexOfLocalPlayer = vectorSize - 1;
			DEBUGONLY(std::cout << "Received join packet with " << vectorSize << " players on the server" << std::endl);
			for (unsigned int i = 0; i < vectorSize; i++) {
				PlayerEntity entity;
				playerJoinResponsePacket >> entity.color.r >> entity.color.g >> entity.color.b >> entity.position.x >> entity.position.y;
				entity.previousPosition = entity.position;
				entity.timestamp = timestamp;
				entity.previousTimestamp = timestamp;
				players.push_back(entity);
			}
			
		}
		
		// Initialize RenderWindow
		sf::RenderWindow window(sf::VideoMode(500, 500), "Bomberman UDP Network Test Client", sf::Style::Titlebar | sf::Style::Close);
		window.setFramerateLimit(60);
		sf::Clock deltaClock;
		float deltaTime;
		while (window.isOpen()) {
			deltaTime = deltaClock.restart().asSeconds();

			// Decrease toggle cooldown
			if (toggleCooldown > 0) {
				toggleCooldown = std::max(0.f, toggleCooldown - deltaTime);
			}

			sf::Event evnt;
			while (window.pollEvent(evnt)) {
				// Handle close event
				if (evnt.type == sf::Event::Closed) {
					sf::Packet playerLeavePacket;
					playerLeavePacket << (short)PLAYER_LEAVE_PACKET_ID << uniqueAccessToken;
					socket.send(playerLeavePacket, server, port);
					DEBUGONLY(std::cout << "Player leave packet sent to server at " << server << " (Size: " << playerLeavePacket.getDataSize() << ")" << std::endl);
					window.close();
				}
			}

			// Close window when [ESC] is pressed
			if (window.hasFocus() && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
				sf::Packet playerLeavePacket;
				playerLeavePacket << (short)PLAYER_LEAVE_PACKET_ID << uniqueAccessToken;
				socket.send(playerLeavePacket, server, port);
				DEBUGONLY(std::cout << "Player leave packet sent to server at " << server << " (Size: " << playerLeavePacket.getDataSize() << ")" << std::endl);
				window.close();
			}

			// Toggle entity interpolation and client prediction
			if (window.hasFocus() && toggleCooldown == 0.f) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
					useEntityInterpolation = !useEntityInterpolation;
					toggleCooldown += 0.2f;
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
					useClientPrediction = !useClientPrediction;
					toggleCooldown += 0.2f;
				}
			}

			// check for, calculate and send movement data
			if (window.hasFocus() && (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))) {
				sf::Vector2f movement(0, 0);
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
					movement.y = -1.f;
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
					movement.y = 1.f;
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
					movement.x = -1.f;
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
					movement.x = 1.f;
				}
				float length = sqrtf(movement.x * movement.x + movement.y * movement.y);
				movement = sf::Vector2f((movement.x / length) * 200.f * deltaTime, (movement.y / length) * 200.f * deltaTime);
				
				if (useClientPrediction) {
					players[indexOfLocalPlayer].position += movement;
				}
				
				sf::Packet playerMovePacket;
				playerMovePacket << (short)PLAYER_MOVE_PACKET_ID << uniqueAccessToken << movement.x << movement.y;
				socket.send(playerMovePacket, server, port);
				DEBUGONLY(std::cout << "Player move packet sent to server at " << server << " (Size: " << playerMovePacket.getDataSize() << ")" << std::endl);
			}

			// Receive Status Update Packet
			sf::Packet packet;
			sf::IpAddress endpoint;
			unsigned short endpointPort;

			socket.setBlocking(false);
			while (socket.receive(packet, endpoint, endpointPort) != sf::Socket::NotReady) {
				// Check Packet Validity and evaluate
				short header;
				packet >> header;

				switch(header) {
				default: {
					DEBUGONLY(std::cout << "Unknown packet (" << packet.getDataSize() << " Bytes) received from server " << endpoint << " with header " << header << std::endl);
						break;
				}
				case PLAYER_STATUS_PACKET_ID: {
					float timestamp;
					int playerEntityCount;
					packet >> timestamp >> playerEntityCount;
					DEBUGONLY(std::cout << "Player status packet (" << packet.getDataSize() << " Bytes) received from server " << endpoint << " with header " << header << ", Player Entity Count: " << playerEntityCount << "\n");

						// Update each entity
						for (unsigned int i = 0; i < playerEntityCount; i++) {
							DEBUGONLY(std::cout << "[" << i + 1 << " out of " << playerEntityCount << "]: ");

							float posx, posy;
							packet >> posx >> posy;

							DEBUGONLY(std::cout << "x: " << posx << ", y: " << posy << std::endl);
							
							if (useClientPrediction && i == indexOfLocalPlayer) {
									continue;
							}

							players[i].previousPosition = players[i].position;
							players[i].previousTimestamp = players[i].timestamp;
							players[i].position.x = posx;
							players[i].position.y = posy;
							players[i].timestamp = timestamp;
						}
					break;
				}
				case PLAYER_JOIN_PACKET_ID: {
					int r, g, b;
					float timestamp, x, y;
					packet >> timestamp >> r >> g >> b >> x >> y;
					DEBUGONLY(std::cout << "Player join packet (" << packet.getDataSize() << " Bytes) received from server " << endpoint << " with color " << r << ", " << g << ", " << b << " at position " << x << ", " << y << std::endl);
					PlayerEntity joinedPlayer(sf::Color(r, g, b), sf::Vector2f(x, y));
					joinedPlayer.timestamp = timestamp;
					joinedPlayer.previousTimestamp = timestamp;
					joinedPlayer.previousPosition = joinedPlayer.position;
					players.push_back(joinedPlayer);
					break;
				}
				case PLAYER_LEAVE_PACKET_ID: {
					int indexOfLeftPlayer;
					packet >> indexOfLeftPlayer;
					DEBUGONLY(std::cout << "Player leave packet (" << packet.getDataSize() << " Bytes) received from server " << endpoint << " with index " << indexOfLeftPlayer << std::endl);

					players.erase(players.begin() + indexOfLeftPlayer);

					if (indexOfLocalPlayer >= indexOfLeftPlayer) {
						--indexOfLocalPlayer;
					}
				}
				}
			}
			//socket.setBlocking(true);

			// Clear window
			window.clear();

			// Draw player entities
			float renderTime = clientClock.getElapsedTime().asMilliseconds() + timestampCorrection - 1000.f / SERVER_TICKRATE;
			for (unsigned int i = 0; i < players.size(); i++) {
				playerStamp.setFillColor(players[i].color);
				// Entity interpolation
				if (useEntityInterpolation && i != indexOfLocalPlayer && renderTime >= players[i].previousTimestamp && renderTime <= players[i].timestamp) {
					float ratio = (renderTime - players[i].previousTimestamp) / (players[i].timestamp - players[i].previousTimestamp);
					float interpolatedX = lerp(players[i].previousPosition.x, players[i].position.x, ratio);
					float interpolatedY = lerp(players[i].previousPosition.y, players[i].position.y, ratio);
					playerStamp.setPosition(sf::Vector2f(interpolatedX, interpolatedY));
				}
				// Normal positioning
				else {
					playerStamp.setPosition(players[i].position);
				}
				window.draw(playerStamp);
			}

			// Draw debug text
			if (useEntityInterpolation) {
				entityInterpolationText.setFillColor(sf::Color::Green);
			}
			else {
				entityInterpolationText.setFillColor(sf::Color::Red);
			}
			if (useClientPrediction) {
				clientPredictionText.setFillColor(sf::Color::Green);
			}
			else {
				clientPredictionText.setFillColor(sf::Color::Red);
			}
			window.draw(entityInterpolationText);
			window.draw(clientPredictionText);

			// Swap buffers
			window.display();
		}
	}

	return EXIT_SUCCESS;
}