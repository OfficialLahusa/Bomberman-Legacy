#pragma once
#include <array>
#include <vector>

namespace sse
{
	namespace nois
	{
		static void PerlinNois1D(size_t count, const std::vector<double> &seed, size_t octaveCount, std::vector<double> &output, double bias = 2.0)
		{
			output.resize(count);
			for (size_t x = 0; x < count; x++)
			{
				double noise = 0.0;
				double scaleAcc = 0.0;
				double scale = 1.0;

				for (size_t o = 0; o < octaveCount; o++)
				{
					size_t pitch = count >> o;
					size_t sample1 = (x / pitch) * pitch;
					size_t sample2 = (sample1 + pitch) % count;

					const double blend = static_cast<double>((x - sample1)) / static_cast<double>(pitch);

					const double sample = (1.0 - blend) * seed[sample1] + blend * seed[sample2];

					noise += sample * scale;
					scaleAcc += scale;
					scale = scale / bias;
				}

				// Scale to seed range
				output[x] = noise / scaleAcc;
			}
		}

		static void PerlinNois1D(size_t count, const std::vector<float> &seed, size_t octaveCount, std::vector<float> &output, float bias = 2.0)
		{
			output.resize(count);
			for (size_t x = 0; x < count; x++)
			{
				float noise = 0.0f;
				float scaleAcc = 0.0f;
				float scale = 1.0f;

				for (size_t o = 0; o < octaveCount; o++)
				{
					size_t pitch = count >> o;
					size_t sample1 = (x / pitch) * pitch;
					size_t sample2 = (sample1 + pitch) % count;

					const float blend = static_cast<float>((x - sample1)) / static_cast<float>(pitch);

					const float sample = (1.0f - blend) * seed[sample1] + blend * seed[sample2];

					noise += sample * scale;
					scaleAcc += scale;
					scale = scale / bias;
				}

				// Scale to seed range
				output[x] = noise / scaleAcc;
			}
		}

		static void PerlinNois2D(size_t width, size_t height, const std::vector<double> &seed, size_t octaveCount, std::vector<double> &output, double bias = 2.0)
		{
			output.resize(width * height);
			for (size_t x = 0; x < width; x++)
			{
				for (size_t y = 0; y < height; y++)
				{

					double noise = 0.0;
					double scaleAcc = 0.0;
					double scale = 1.0;

					for (size_t o = 0; o < octaveCount; o++)
					{
						size_t pitch = width >> o;
						size_t sampleX1 = (x / pitch) * pitch;
						size_t sampleY1 = (y / pitch) * pitch;

						size_t sampleX2 = (sampleX1 + pitch) % width;
						size_t sampleY2 = (sampleY1 + pitch) % width;

						const double blendX = static_cast<double>(x - sampleX1) / static_cast<double>(pitch);
						const double blendY = static_cast<double>(y - sampleY1) / static_cast<double>(pitch);
							   
						const double sampleT = (1.0 - blendX) * seed[sampleY1 * width + sampleX1] + blendX * seed[sampleY1 * width + sampleX2];
						const double sampleB = (1.0 - blendX) * seed[sampleY2 * width + sampleX1] + blendX * seed[sampleY2 * width + sampleX2];

						scaleAcc += scale;
						noise += (blendY * (sampleB - sampleT) + sampleT) * scale;
						scale = scale / bias;
					}

					output[y * width + x] = noise / scaleAcc;
				}
			}
		}


		static void PerlinNois2D(size_t width, size_t height, const std::vector<float> &seed, size_t octaveCount, std::vector<float> &output, float bias = 2.0)
		{
			output.resize(width * height);
			for (size_t x = 0; x < width; x++)
			{
				for (size_t y = 0; y < height; y++)
				{

					float noise = 0.0f;
					float scaleAcc = 0.0f;
					float scale = 1.0f;

					for (size_t o = 0; o < octaveCount; o++)
					{
						size_t pitch = width >> o;
						size_t sampleX1 = (x / pitch) * pitch;
						size_t sampleY1 = (y / pitch) * pitch;

						size_t sampleX2 = (sampleX1 + pitch) % width;
						size_t sampleY2 = (sampleY1 + pitch) % width;

						const float blendX = static_cast<float>(x - sampleX1) / static_cast<float>(pitch);
						const float blendY = static_cast<float>(y - sampleY1) / static_cast<float>(pitch);

						const float sampleT = (1.0f - blendX) * seed[sampleY1 * width + sampleX1] + blendX * seed[sampleY1 * width + sampleX2];
						const float sampleB = (1.0f - blendX) * seed[sampleY2 * width + sampleX1] + blendX * seed[sampleY2 * width + sampleX2];

						scaleAcc += scale;
						noise += (blendY * (sampleB - sampleT) + sampleT) * scale;
						scale = scale / bias;
					}

					output[y * width + x] = noise / scaleAcc;
				}
			}
		}

		


	} // namespace nois
}  // namespace sse
