#pragma once

#define NOISE_STATIC
#include "noise/noise.h"
#include "noiseutils.h"
#include <time.h>

#include "Math.hpp"

class TerrainGenerator {
private:
	module::Perlin myModule;
	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
	utils::RendererImage renderer;
	utils::Image image;
	utils::WriterBMP writer;

public:
	utils::Image* GetTerrain(const Vec2& chunkPos, long long& seed, int CHUNK_WIDTH, int CHUNK_HEIGHT);
};
