#pragma once
#include <vector>

#include "../shaders/Shader.h"
#include "Material.h"

class Object
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
	Object(Material material, glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scaleFactor);

	void Render(const Shader& shader, bool wireframeMode);

	void RenderDepth(const Shader& shader);

	std::vector<float> GetVerticesInWorldSpace();

protected:
	void initialize();

protected:
	unsigned int vertexCount = 0;
	unsigned int indexCount = 0;

private:
	void calculateTangents();
	void translate(glm::vec3 translation);
	void scale(glm::vec3 scaleFactor);
	void rotate(glm::vec3 eulerAngles);

private:
	unsigned int VAO = 0;
	unsigned int EBO = 0;
	unsigned int VBO_VERTICES = 0;
	unsigned int VBO_NORMALS = 0;
	unsigned int VBO_UVS = 0;
	unsigned int VBO_TANGENTS = 0;
};

