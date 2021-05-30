#pragma once

#include "../shaders/Shader.h"
#include "Material.h"

class Terrain
{
public:
	glm::mat4 transform = glm::mat4(1.0f);

	Material material;
	float* vertices;
	float* normals;
	float* uvs;
	unsigned int* indices;
	float* tangents;

public:
	Terrain(Material material, glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scaleFactor);

	void Render(const Shader& shader, bool wireframeMode);

private:
	void initialize();
	void calculateTangents();

	void translate(glm::vec3 translation);
	void scale(glm::vec3 scaleFactor);
	void rotate(glm::vec3 eulerAngles);

private:
	unsigned int vertexCount = 0;
	unsigned int indexCount = 0;

	unsigned int VAO = 0;
	unsigned int EBO = 0;
	unsigned int VBO_VERTICES = 0;
	unsigned int VBO_NORMALS = 0;
	unsigned int VBO_UVS = 0;
	unsigned int VBO_TANGENTS = 0;
};

