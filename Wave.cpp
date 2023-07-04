#include "Wave.h"


DoubleDeque<float>* generate(int width, int height, float scale, std::vector<Wave>* waves, float offx, float offy)
{
	perlinNoise perlin;

	DoubleDeque<float>* noiseMap = new DoubleDeque<float>(width, height);

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			float samplePosX = (float)x * scale * offx;
			float samplePosY = (float)y * scale * offy;

			float normalization = 0.0f;

			for (auto wave : *waves)
			{
				*noiseMap->get(x, y) += wave.amplitude * perlin.noise(samplePosX * wave.frequency + wave.seed, samplePosY * wave.frequency + wave.seed, wave.frequency + wave.seed);
				normalization += wave.amplitude;
			}
			*noiseMap->get(x, y) /= normalization;
		}
	}

	return noiseMap;
}