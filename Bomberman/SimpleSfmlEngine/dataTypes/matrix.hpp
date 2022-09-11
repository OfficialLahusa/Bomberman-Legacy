#pragma once
#include <array>
#include <functional>
#include <string>
#include <vector>

namespace sse
{
	template<typename T, size_t T_X, size_t T_Y, bool allocateOnStack = false>
	class Matrix
	{
	public: 
		Matrix()
			:m_data(T_X * T_Y)
		{
		}

		T at(size_t x, size_t y) const { return m_data[y * T_X + x]; }

		void set(size_t x, size_t y, T value) { m_data[y * T_X + x] = value; }

		size_t x() const { return T_X; }
		size_t y() const { return T_Y; }

		void print()  const
		{
			std::string s;
			s += "Matrix(";
			s += std::to_string(T_X) + ", ";
			s += std::to_string(T_Y) + ")[";
			for (size_t y = 0; y < T_Y; y++)
			{
				s += "\ny = ";
				s += std::to_string(y);
				s += "[";
				for (size_t x = 0; x < T_X - 1; x++)
				{
					s += std::to_string(at(x, y)) + ", ";
				}
				s += std::to_string(at(T_X - 1, y)) + "]";
			}
			s += "]\n";
			std::puts(s.c_str());
			fflush(stdout);
		}

		void loopThrough(std::function<inline void(size_t x, size_t y, T& val)> func)
		{
			for (size_t x = 0; x < T_X; x++)
			{
				for (size_t y = 0; y < T_Y; y++)
				{
					func(x, y, m_data[y * T_X + x]);
				}
			}
		}

	private:
		std::vector<T> m_data;
	};

	template<typename T, size_t T_X, size_t T_Y>
	class Matrix<T, T_X, T_Y, false>
	{
	public:

		T at(size_t x, size_t y) const {
			return m_data[y * T_X + x];
		}

		void set(size_t x, size_t y, T value) {
			m_data[y * T_X + x] = value;
		}

		size_t x() const { return T_X; }
		size_t y() const { return T_Y; }

		void print() const
		{
			std::string s;
			s += "Matrix(";
			s += std::to_string(T_X) + ", ";
			s += std::to_string(T_Y) + ")[";
			for (size_t y = 0; y < T_Y; y++)
			{
				s += "\ny = ";
				s += std::to_string(y);
				s += "[";
				for (size_t x = 0; x < T_X - 1; x++)
				{
					s += std::to_string(at(x, y)) + ", ";
				}
				s += std::to_string(at(T_X - 1, y)) + "]";
			}
			s += "]\n";
			std::puts(s.c_str());
			fflush(stdout);
		}

		void loopThrough(std::function<inline void(size_t x, size_t y, T& val)> func)
		{
			for (size_t x = 0; x < T_X; x++)
			{
				for (size_t y = 0; y < T_Y; y++)
				{
					func(x, y, m_data[y * T_X + x]);
				}
			}
		}

	private:
		std::array<T, T_X * T_Y> m_data;
	};
}  // namespace sse