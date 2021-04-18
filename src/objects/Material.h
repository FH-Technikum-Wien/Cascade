#pragma once
#include <glm/vec3.hpp>

#include <glad\glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb-image/stb_image.h"

#include <iostream>

class Material
{
public:
	unsigned int texture = 0;
	unsigned int normalMap = 0;
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	float ambientStrength = 1;
	float diffuseStrength = 1;
	float specularStrength = 0;
	float focus = 64;

public:
	Material() = default;

	Material(const char* texturePath, const char* normalMapPath, unsigned int colorFormat)
	{
		this->texture = LoadTexture(texturePath, colorFormat);
		this->normalMap = LoadTexture(normalMapPath, colorFormat);
	}

	unsigned int LoadTexture(const char* path, unsigned int colorFormat)
	{
		stbi_set_flip_vertically_on_load(true);
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		// Set texture wrapping options. S == x-axis | T == y-axis
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, numberOfChannels;
		unsigned char* data = stbi_load(path, &width, &height, &numberOfChannels, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, colorFormat, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}

		stbi_image_free(data);
		return texture;
	}
};