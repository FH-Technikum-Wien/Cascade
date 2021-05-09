#pragma once

#include "../shaders/Shader.h"
#include "Material.h"

class Plane
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
	Plane() = default;

	Plane(Material material, glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scale);

	void Render(const Shader& shader, bool wireframeMode);

private:
	unsigned int VAO = 0;
	unsigned int EBO = 0;
	unsigned int VBO_VERTICES = 0;
	unsigned int VBO_NORMALS = 0;
	unsigned int VBO_UVS = 0;
	unsigned int VBO_TANGENTS = 0;

	unsigned int vertexCount = 6;
	unsigned int indexCount = 6;

private:
	void initialize();

	void calculateTangents();

	void translate(glm::vec3 translation);

	void rotate(glm::vec3 eulerAngles);
};