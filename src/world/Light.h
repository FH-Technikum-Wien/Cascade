#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../shaders/Shader.h"

class Light
{
public:
	glm::vec3 position;
	glm::vec3 color;
	float intensity = 1;

	Light() = default;
	Light(glm::vec3 position, float intensity = 1, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f))
	{
		this->position = position;
		this->color = color;
		this->intensity = intensity;
	}

	void activateLight(Shader& shader)
	{
		shader.activate();
		shader.setFloat("lightIntensity", intensity);
		shader.setVec3("lightColor", color);
		shader.setVec3("lightPos", position);
	}
};
