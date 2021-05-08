#pragma once
#include <glm/vec3.hpp>

class Material
{
public:
	unsigned int texture = 0;
	unsigned int normalMap = 0;
	unsigned int displacementMap = 0;
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	float ambientStrength = 1;
	float diffuseStrength = 1;
	float specularStrength = 0;
	float focus = 64;

public:
	Material() = default;

	Material(const char* texturePath, unsigned int colorFormat)
	{
		this->texture = LoadTexture(texturePath, colorFormat);
	}

	Material(const char* texturePath, const char* normalMapPath, unsigned int colorFormat)
	{
		this->texture = LoadTexture(texturePath, colorFormat);
		this->normalMap = LoadTexture(normalMapPath, colorFormat);
	}

	Material(const char* texturePath, const char* normalMapPath, const char* displacementMapPath, unsigned int colorFormat)
	{
		this->texture = LoadTexture(texturePath, colorFormat);
		this->normalMap = LoadTexture(normalMapPath, colorFormat);
		this->displacementMap = LoadTexture(displacementMapPath, colorFormat);
	}

	unsigned int LoadTexture(const char* path, unsigned int colorFormat);
};