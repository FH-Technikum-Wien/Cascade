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
	glm::mat4 lightSpaceMat;

	Light() = default;
	Light(glm::vec3 position, float intensity = 1, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f))
	{
		this->position = position;
		this->color = color;
		this->intensity = intensity;

		float nearPlane = 0.001f, farPlane = 100.0f;
		glm::mat4 lightProjection;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);

		glm::mat4 lightViewMat = glm::lookAt(position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		// Transforms world space to light space.
		lightSpaceMat = lightProjection * lightViewMat;
	}
};
