#include <noise/noise.h>
#include "noiseutils.h"
#include "VoxelNoise.hpp"
#include <noise/noise.h>
#include "noiseutils.h"
using namespace noise;
utils::NoiseMap heightMap;

std::string GenerateTerrainMap(int resolution)
{
	std::string filename = "Resources/heightmap/terrain_heightmap.bmp";

	module::Perlin perlin_mod;

	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule(perlin_mod);
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

	return filename;
}