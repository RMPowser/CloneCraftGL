#include "TerrainGenerator.h"

noise::utils::Image* TerrainGenerator::GetTerrain(const Vec2& chunkPos, long long& seed, int CHUNK_WIDTH, int CHUNK_HEIGHT) {
	float scalar = 25;

	// define the boundaries of the part of the terrain we want to render
	float lowBoundX = chunkPos.x / scalar;
	float highBoundX = (chunkPos.x + 1) / scalar;
	float lowBoundZ = chunkPos.z / scalar;
	float highBoundZ = (chunkPos.z + 1) / scalar;

	// if there is no seed, create one from the current time
	if (seed == -1) {
		seed = time(NULL);
	}

	// apply the seed
	myModule.SetSeed(seed);

	// create the height map
	heightMapBuilder.SetSourceModule(myModule);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetDestSize(CHUNK_WIDTH, CHUNK_WIDTH);
	heightMapBuilder.SetBounds(lowBoundX, highBoundX, lowBoundZ, highBoundZ);
	heightMapBuilder.Build();

	// render the height map to an image
	renderer.SetSourceNoiseMap(heightMap);
	renderer.SetDestImage(image);
	renderer.Render();

	// normalize the image to be within acceptable world coordinates. ie: no terrain above a threshold
	for (int x = 0; x < image.GetWidth(); x++) {
		for (int y = 0; y < image.GetHeight(); y++) {
			utils::Color c = image.GetValue(x, y);
			c.red /= 2;
			c.green /= 2;
			c.blue /= 2;
			c.alpha = 255;
			image.SetValue(x, y, c);
		}
	}

	// optionally write it to a bmp file for debugging
	//writer.SetSourceImage(image);
	//writer.SetDestFilename("terrain.bmp");
	//writer.WriteDestFile();

	return &image;
}

