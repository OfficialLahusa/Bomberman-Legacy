#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace sse
{
	class AnimatedSprite : public sf::Sprite
	{
	public:
		struct Frame
		{
			Frame(sf::Texture& texture, float delay)
				: m_texture(&texture)
				, m_delay(delay)
			{ }
			sf::Texture* m_texture;
			float m_delay;
		};

		std::vector<AnimatedSprite::Frame>& operator[](const std::string& s)
		{
			return m_animations[s];
		}
		//void AddAnimation(std::string name);

		void SetActiveAnimation(std::string str)
		{
			m_currentAnimation = std::move(str);
		}

		void Update()
		{
			setTexture(*m_animations[m_currentAnimation][m_frameCounter].m_texture);
			if (!m_currentAnimation.empty())
			{
				if (m_currentAnimation.find(m_currentAnimation) != m_currentAnimation.back())
				{
					if (m_clock.getElapsedTime().asSeconds() >= m_animations[m_currentAnimation][m_frameCounter].m_delay)
					{
					
						if (m_animations[m_currentAnimation].size() - 1 > m_frameCounter)
						{
							m_frameCounter++;
						}
						else
						{
							m_frameCounter = 0;
						}

						setTexture(*m_animations[m_currentAnimation][m_frameCounter].m_texture);
						m_clock.restart();
					}					
				}
			}
		}

	private:
		std::map<std::string, std::vector<AnimatedSprite::Frame> > m_animations;
		size_t m_frameCounter = 0;
		std::string m_currentAnimation = "";
		sf::Clock m_clock;
	};
}  // namespace sse