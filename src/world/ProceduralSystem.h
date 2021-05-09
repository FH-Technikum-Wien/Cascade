#pragma once
#include "../shaders/Shader.h"
#include "Camera.h"
#include "Chunk.h"
#include <vector>

class ProceduralSystem
{
public:
	ProceduralSystem(float screenWidth, float screenHeight, const Camera& camera);

	void Update(const Camera& camera);

private:
	const char* VERTEX_SHADER_PATH = "src/shaders/procedural/shader.vert";
	const char* FRAGMENT_SHADER_PATH = "src/shaders/procedural/shader.frag";
	const char* GEOMETRY_SHADER_PATH = "src/shaders/procedural/shader.geom";

	const char* NOISE_VERTEX_SHADER_PATH = "src/shaders/procedural/noiseShader.vert";
	const char* NOISE_FRAGMENT_SHADER_PATH = "src/shaders/procedural/noiseShader.frag";

	// Defines the density texture size
	const int TEXTURE_WIDTH = 64;
	const int TEXTURE_HEIGHT = 64;
	const int TEXTURE_DEPTH = 64;
	// Defines the threshold for the marching cube algorithm -> What will be treated as solid
	const float SOLID_THRESHOLD = 0.51f;

	// Size of each chunk
	ChunkDimensions chunkDimensions = { TEXTURE_WIDTH, TEXTURE_HEIGHT, TEXTURE_DEPTH };
	// List of all displayed chunks. Currently three -> Below, Current, Above
	std::vector<Chunk> chunks = std::vector<Chunk>();

	Shader shader = Shader();
	Shader noiseShader = Shader();

	unsigned int VAO = 0;
	unsigned int VBO = 0;

	bool wireframeModeActive = false;

	// Used for rendering on each z-layer of the 3D texture
	float rectangle[12] =
	{
		-1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f, -1.0f,

		 1.0f,  1.0f,
		-1.0f,  1.0f,
		 1.0f, -1.0f,
	};
};

