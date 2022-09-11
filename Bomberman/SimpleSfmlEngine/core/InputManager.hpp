#pragma once

#include <SFML/Graphics.hpp>

namespace sse
{
	class InputManager
	{
	public:
		InputManager() = default;
		~InputManager() = default;

		sf::Vector2f getPointInView(const sf::RenderWindow& window, sf::Vector2i point) {
			sf::Vector2f mouse_pos = window.mapPixelToCoords(point);
			return mouse_pos;
		}

		bool IsPointOnSprite(const sf::Sprite& object, sf::Vector2i point)
		{
			sf::IntRect tempRect(static_cast<int>(object.getPosition().x), static_cast<int>(object.getPosition().y), static_cast<int>(object.getGlobalBounds().width), static_cast<int>(object.getGlobalBounds().height));
			return tempRect.contains(point);
		}

		bool IsSpriteClicked(const sf::Sprite& object, sf::Mouse::Button button, const sf::RenderWindow &window)
		{
			if (sf::Mouse::isButtonPressed(button))
			{
				sf::IntRect tempRect(static_cast<int>(object.getPosition().x), static_cast<int>(object.getPosition().y), static_cast<int>(object.getGlobalBounds().width), static_cast<int>(object.getGlobalBounds().height));
				sf::Vector2f mousePos = getPointInView(window, sf::Mouse::getPosition());
				if (tempRect.contains(mousePos.x, mousePos.y));
				{
					return true;
				}

			}
			return false;
		}

		bool IsCursorOnSprite(const sf::Sprite& object, const sf::RenderWindow &window)
		{
			sf::IntRect tempRect(static_cast<int>(object.getPosition().x), static_cast<int>(object.getPosition().y), static_cast<int>(object.getGlobalBounds().width), static_cast<int>(object.getGlobalBounds().height));
			sf::Vector2f mousePos = getPointInView(window, sf::Mouse::getPosition());
			return tempRect.contains(mousePos.x, mousePos.y);
		}

		sf::Vector2i GetCursorPosition(sf::RenderWindow & /*window*/)
		{
			return sf::Mouse::getPosition();
		}

	private:

	};


} // namespace sse
