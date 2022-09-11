#pragma once
#include <random>

namespace sse
{
		class rng
		{
		private:
			std::random_device randomDevice;
			std::mt19937 mt;

		public:

			rng()
				:  mt(randomDevice())
			{

			}

			explicit rng(unsigned int seed)
				:  mt(seed)
			{

			}

			template<typename T>
			T randomInteger()
			{
				std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
				return dist(mt);
			}

			template<typename T>
			T randomInteger(T min, T max)
			{
				std::uniform_int_distribution<T> dist(min, max);
				return dist(mt);
			}

			template<typename T>
			T randomReal()
			{
				std::uniform_real_distribution<T> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
				return dist(mt);
			}

			template<typename T>
			T randomReal(T min, T max)
			{
				std::uniform_real_distribution<T> dist(min, max);
				return dist(mt);
			}
		};
} // namespace sse