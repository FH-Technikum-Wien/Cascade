#include "Chunk.h"
#include <glm/gtx/transform.hpp>
#include "input.h"

Chunk::Chunk(ChunkDimensions dimension, float chunkHeight, Shader& shader, unsigned int& VAO, float screenWidth, float screenHeight)
{
	m_dimensions = dimension;
	ChunkHeight = chunkHeight;
	m_VAO = VAO;
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	// Create 3D texture
	glGenTextures(1, &m_texture3D);
	glBindTexture(GL_TEXTURE_3D, m_texture3D);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// GL_R32F -> 32-bit float as internal format (red 32-bit float)
	// GL_RED -> Data at red (rgb)
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, m_dimensions.Width, m_dimensions.Height, m_dimensions.Depth, 0, GL_RED, GL_FLOAT, nullptr);
	// Create framebuffer for data
	glGenFramebuffers(1, &m_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	GLenum attachment = GL_COLOR_ATTACHMENT0;
	glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_3D, m_texture3D, 0, 0);
	glDrawBuffers(1, &attachment);
	// Add texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, m_texture3D);
	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	RenderTexture3D(shader);
}

void Chunk::UpdateTexture3D(Shader& shader, float newChunkHeight)
{
	// Update chunk height and render density texture with it
	ChunkHeight = newChunkHeight;
	RenderTexture3D(shader);
}

void Chunk::RenderPoints(Shader& shader, bool wireframeMode) const
{
	shader.activate();
	shader.setFloat("chunkHeight", ChunkHeight);

	// Add texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, m_texture3D);

	// Set render mode to wireframe
	if (wireframeMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_POINTS, 0, m_dimensions.Width * m_dimensions.Height * m_dimensions.Depth);
	// Unbind
	glBindVertexArray(0);
	// Reset render mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Chunk::RenderTexture3D(Shader& shader) const
{
	// Set viewport to fit texture before rendering
	glViewport(0, 0, m_dimensions.Width, m_dimensions.Height);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	shader.activate();
	float stepSize = 8.0f / 62.0f;
	shader.setFloat("chunkHeight", ChunkHeight * stepSize);

	// Draw rectangles on each layer
	for (int i = 0; i < m_dimensions.Depth; i++)
	{
		shader.setFloat("layer", (float)i / (m_dimensions.Depth - 1));
		
		// Bind current layer
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
		glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, m_texture3D, 0, i);
		// Draw points on each layer
		glBindVertexArray(m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	// Unbind
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Reset Viewport
	glViewport(0, 0, m_screenWidth, m_screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
