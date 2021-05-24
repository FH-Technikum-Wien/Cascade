#include "ProceduralSystem.h"

ProceduralSystem::ProceduralSystem(float screenWidth, float screenHeight, const Camera& camera)
{
	shader.addShader(VERTEX_SHADER_PATH, VERTEX_SHADER);
	shader.addShader(FRAGMENT_SHADER_PATH, FRAGMENT_SHADER);
	shader.addShader(GEOMETRY_SHADER_PATH, GEOMETRY_SHADER);

	shader.activate();
	// Set values that will not change
	shader.setMat4("projectionMat", camera.ProjectionMat);
	shader.setInt("width", TEXTURE_WIDTH);
	shader.setInt("height", TEXTURE_HEIGHT);
	shader.setInt("depth", TEXTURE_DEPTH);
	shader.setFloat("solidThreshold", SOLID_THRESHOLD);

	//----------------
	// Vertex data
	//----------------
	noiseShader.addShader(NOISE_VERTEX_SHADER_PATH, VERTEX_SHADER);
	noiseShader.addShader(NOISE_FRAGMENT_SHADER_PATH, FRAGMENT_SHADER);

	glGenVertexArrays(1, &VAO);
	// Bind Vertex-Array-Object to configure VBO(s)
	glBindVertexArray(VAO);
	// Generate Vertex-Buffer-Cube to manage memory on GPU and store vertices
	glGenBuffers(1, &VBO);
	// Bind Vertex-Buffer-Cube to configure it
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Copy vertex data into buffer's memory (into VBO which is bound to GL_ARRAY_BUFFER)
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), rectangle, GL_STATIC_DRAW);

	// Tell OpenGL how to interpret/read the vertex data (per vertex attribute, e.g. one vertex point)
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Add three chunks for startup
	chunks.push_back(Chunk(chunkDimensions, -TEXTURE_HEIGHT, noiseShader, VAO, screenWidth, screenHeight));
	chunks.push_back(Chunk(chunkDimensions, 0, noiseShader, VAO, screenWidth, screenHeight));
	chunks.push_back(Chunk(chunkDimensions, TEXTURE_HEIGHT, noiseShader, VAO, screenWidth, screenHeight));
}

void ProceduralSystem::Update(const Camera& camera, bool wireframeMode)
{
	glDisable(GL_CULL_FACE);

	// Change furthest chunk to be the next one
	if (camera.Position.y > chunks[2].ChunkHeight)
	{
		// Switch positions, so the lowest chunk becomes the highest
		std::swap(chunks[0], chunks[1]);
		std::swap(chunks[1], chunks[2]);
		// Set new chunk height and update it's density texture
		chunks[2].UpdateTexture3D(noiseShader, chunks[1].ChunkHeight + TEXTURE_HEIGHT);
	}
	else if (camera.Position.y < chunks[1].ChunkHeight)
	{
		// Switch positions, so the highest chunk becomes the lowest
		std::swap(chunks[2], chunks[1]);
		std::swap(chunks[1], chunks[0]);
		// Set new chunk height and update it's density texture
		chunks[0].UpdateTexture3D(noiseShader, chunks[1].ChunkHeight - TEXTURE_HEIGHT);
	}

	shader.activate();
	shader.setMat4("viewMat", camera.GetViewMat());

	// Render each chunk using marching cubes 
	for (const Chunk& chunk : chunks)
		chunk.RenderPoints(shader, wireframeMode);

	glEnable(GL_CULL_FACE);
}
