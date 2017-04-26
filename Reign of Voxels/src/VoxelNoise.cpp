#include <noise/noise.h>
#include "noiseutils.h"

#include "VoxelNoise.hpp"

using namespace noise;
utils::NoiseMap heightMap;

static module::Perlin g_perlinMod;
static sf::Image *g_heightMap;
static sf::Image *g_colorMap;

std::string GenerateTerrainMap(int resolution)
{
	std::string heightMapName = "Resources/heightmap/terrain_heightmap.bmp";
	std::string colorMapName = "Resources/heightmap/terrain_colormap.bmp";

	int mapResolution = resolution + 1;

	g_perlinMod.SetSeed(rand() % resolution);
	g_perlinMod.SetOctaveCount(4);
	g_perlinMod.SetFrequency(.5f);
	g_perlinMod.SetLacunarity(1.9524f);
	g_perlinMod.SetPersistence(0.68324f);

	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;

	heightMapBuilder.SetSourceModule(g_perlinMod);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetDestSize(mapResolution, mapResolution);
	heightMapBuilder.SetBounds(2.0, 6.0, 1.0, 5.0);
	heightMapBuilder.Build();


	utils::RendererImage renderer;
	utils::Image heightMapImage;
	utils::Image colorMapImage;

	renderer.SetSourceNoiseMap(heightMap);
	renderer.SetDestImage(heightMapImage);
	renderer.Render();

	utils::WriterBMP writer;
	writer.SetSourceImage(heightMapImage);
	writer.SetDestFilename(heightMapName);
	writer.WriteDestFile();


	renderer.SetDestImage(colorMapImage);
	renderer.ClearGradient();
	renderer.AddGradientPoint(-1.0000, utils::Color(0, 0, 255, 255)); // deeps
	renderer.AddGradientPoint(-0.5000, utils::Color(0, 255, 0, 255)); // shore
	renderer.AddGradientPoint(0.0625, utils::Color(0, 255, 0, 255)); // sand
	renderer.AddGradientPoint(0.250, utils::Color(0, 255, 0, 255)); // grass
	renderer.AddGradientPoint(1.0000, utils::Color(255, 0, 0, 255)); // snow

	renderer.Render();

	writer.SetSourceImage(colorMapImage);
	writer.SetDestFilename(colorMapName);
	writer.WriteDestFile();

	g_heightMap = new sf::Image();
	if (!g_heightMap->loadFromFile(heightMapName.c_str()))
	{
		return 0;
	}

	g_colorMap = new sf::Image();
	if (!g_colorMap->loadFromFile(colorMapName.c_str()))
	{
		return 0;
	}
	return heightMapName;
}

float GetPerlinMapValue(float x, float z)
{
	return (float)g_heightMap->getPixel(x, z).r;
}

sf::Color GetPerlinColorValue(float x, float z)
{
	return g_colorMap->getPixel(x, z);
}