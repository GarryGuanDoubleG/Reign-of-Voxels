#include <noise/noise.h>
#include "noiseutils.h"

#include "VoxelNoise.hpp"

#define HEIGHT_MAP_NAME "Resources/heightmap/terrain_heightmap.bmp"
#define COLOR_MAP_NAME "Resources/heightmap/terrain_colormap.bmp"

using namespace noise;
utils::NoiseMap heightMap;

static module::Perlin g_perlinMod;
static sf::Image *g_heightMap;
static sf::Image *g_colorMap;



std::string GenerateTerrainMap(int resolution)
{
	int mapResolution = resolution + 1;

	module::Perlin mountainTerrain;
	mountainTerrain.SetLacunarity(1.5f);
	mountainTerrain.SetPersistence(.25f);
	mountainTerrain.SetFrequency(2.0f);


	module::Billow baseFlatTerrain;
	baseFlatTerrain.SetFrequency(2.0);
	baseFlatTerrain.SetOctaveCount(2.0f);
	baseFlatTerrain.SetPersistence(2.0f);


	module::ScaleBias flatTerrain;
	flatTerrain.SetSourceModule(0, baseFlatTerrain);
	flatTerrain.SetScale(0.0125);
	flatTerrain.SetBias(-1.0);

	module::Perlin terrainType;
	terrainType.SetFrequency(0.5);
	terrainType.SetPersistence(0.55);

	module::Select finalTerrain;
	finalTerrain.SetSourceModule(0, flatTerrain);
	finalTerrain.SetSourceModule(1, mountainTerrain);
	finalTerrain.SetControlModule(terrainType);
	finalTerrain.SetBounds(0.0, 1000.0);
	finalTerrain.SetEdgeFalloff(0.625);

	//finalTerrain.SetEdgeFalloff(0.125);

	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;



	heightMapBuilder.SetSourceModule(finalTerrain);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetDestSize(mapResolution, mapResolution);
	heightMapBuilder.SetBounds(2.0, 6.0, 1.0, 5.0);
	heightMapBuilder.Build();


	utils::RendererImage renderer;
	utils::Image image;

	renderer.SetSourceNoiseMap(heightMap);
	renderer.SetDestImage(image);
	renderer.Render();

	utils::WriterBMP writer;
	writer.SetSourceImage(image);
	writer.SetDestFilename(HEIGHT_MAP_NAME);
	writer.WriteDestFile();

	renderer.SetSourceNoiseMap(heightMap);
	renderer.SetDestImage(image);
	renderer.ClearGradient();
	renderer.AddGradientPoint(-1.00, utils::Color(32, 160, 0, 255)); // grass
	renderer.AddGradientPoint(-0.25, utils::Color(224, 224, 0, 255)); // dirt
	renderer.AddGradientPoint(0.25, utils::Color(128, 128, 128, 255)); // rock
	renderer.AddGradientPoint(1.00, utils::Color(255, 255, 255, 255)); // snow
	renderer.EnableLight();
	renderer.SetLightContrast(3.0);
	renderer.SetLightBrightness(2.0);
	renderer.Render();

	writer.SetSourceImage(image);
	writer.SetDestFilename(COLOR_MAP_NAME);
	writer.WriteDestFile();

	g_heightMap = new sf::Image();
	if (!g_heightMap->loadFromFile(HEIGHT_MAP_NAME))
	{
		return 0;
	}

	g_colorMap = new sf::Image();
	if (!g_colorMap->loadFromFile(COLOR_MAP_NAME))
	{
		return 0;
	}

	return HEIGHT_MAP_NAME;
}


float GetPerlinMapValue(float x, float z)
{
	return (float)g_heightMap->getPixel(x, z).r;
}

sf::Color GetPerlinColorValue(float x, float z)
{
	return g_colorMap->getPixel(x, z);
}
