#include <noise/noise.h>
#include "noiseutils.h"

#include "VoxelNoise.hpp"

using namespace noise;
utils::NoiseMap heightMap;

static module::Perlin g_perlinMod;
static sf::Image *g_heightMap;

std::string GenerateTerrainMap(int resolution)
{
	std::string filename = "Resources/heightmap/terrain_heightmap.bmp";

	g_perlinMod.SetOctaveCount(4);
	g_perlinMod.SetFrequency(.5f);
	g_perlinMod.SetLacunarity(2.2324f);
	g_perlinMod.SetPersistence(0.68324f);

	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;

	heightMapBuilder.SetSourceModule(g_perlinMod);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetDestSize(resolution, resolution);
	heightMapBuilder.SetBounds(2.0, 6.0, 1.0, 5.0);
	heightMapBuilder.Build();


	utils::RendererImage renderer;
	utils::Image image;
	
	renderer.SetSourceNoiseMap(heightMap);
	renderer.SetDestImage(image);
	renderer.Render();

	utils::WriterBMP writer;
	writer.SetSourceImage(image);
	writer.SetDestFilename(filename);
	writer.WriteDestFile();

	g_heightMap = new sf::Image();
	if (!g_heightMap->loadFromFile(filename.c_str()))
	{
		return 0;
	}

	return filename;
}

float GetPerlinMapValue(float x, float z)
{
	return (float)g_heightMap->getPixel(x, z).r;
}
