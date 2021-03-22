#pragma once
#include "../shaders/shader.h"
#include <iostream>

struct ChunkDimensions
{
	int Width = 0;
	int Height = 0;
	int Depth = 0;
};

class Chunk
{
public:

	Chunk() = default;

	explicit Chunk(ChunkDimensions dimensions, float chunkHeight, Shader& shader, unsigned int& VAO, float screenWidth, float screenHeight);

	void UpdateTexture3D(Shader& shader, float newChunkHeight);
	void RenderPoints(Shader& shader, bool wireframeMode) const;
	void RenderTexture3D(Shader& shader) const;


public:
	float ChunkHeight;

private:
	void printError() const;
	

private:

	ChunkDimensions m_dimensions;

	unsigned int m_VAO = -1;
	unsigned int m_frameBuffer = -1;
	unsigned int m_texture3D = -1;

	int m_screenWidth;
	int m_screenHeight;
};

