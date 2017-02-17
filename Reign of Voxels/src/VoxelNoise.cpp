#include "VoxelNoise.hpp"

const char * GeneratePerlin(int resolution)
{
	const char * filename = "Resources/heightmap/terrain_generator.bmp";

	module::Perlin perlin_mod;

	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule(perlin_mod);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetDestSize(resolution, resolution);

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