/*
* Generate noise map from perlin noise function
* Implementation of: https://gamedevacademy.org/procedural-2d-maps-unity-tutorial/
*/

#pragma once

#include "PerlinNoise.h"
#include "DoubleDeque.h"

struct Wave
{
	float seed;
	float frequency;
	float amplitude;
};

// Generate a noise map using perlin noise
DoubleDeque<float>* generate(int width, int height, float scale, std::vector<Wave>* waves, float offx, float offy);

