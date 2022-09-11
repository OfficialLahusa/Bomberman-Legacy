#include <random>

namespace sse
{
	namespace random
	{
		static std::random_device randomDevice;
		static std::mt19937 mt(randomDevice());


		template<typename T>
		static T randomInteger()
		{
			std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
			return dist(mt);
		}

		template<typename T>
		static T randomInteger(T min, T max)
		{
			std::uniform_int_distribution<T> dist(min, max);
			return dist(mt);
		}

		template<typename T>
		static T randomReal()
		{
			std::uniform_real_distribution<T> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
			return dist(mt);
		}

		template<typename T>
		static T randomReal(T min, T max)
		{
			std::uniform_real_distribution<T> dist(min, max);
			return dist(mt);
		}
	}  // namespace random
} // namespace sse