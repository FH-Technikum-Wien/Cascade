#pragma once
#include <glm/vec3.hpp>

#include "../objects/Material.h"
#include "../shaders/Shader.h"
#include "../util/Random.h"

class ParticleSystem
{
public:
	glm::vec3 SpawnPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 VelocityMin = glm::vec3(-5, 0, -5);
	glm::vec3 VelocityRange = glm::vec3(5, 20, 5);
	glm::vec3 Gravity = glm::vec3(0, -5, 0);
	glm::vec3 Color = glm::vec3(1.0f, 0.0f, 0.0f);
	float Size = 0.1f;
	float LifetimeMin = 1;
	float LifetimeRange = 5;
	float SpawnFrequence = 0.5f;
	int NumberOfParticlesToSpawn = 10;

public:
	ParticleSystem(const Shader& updateShader, const Shader& renderShader, Material material);

	void Update(const Shader& updateShader, float deltaTime);
	void Render(const Shader& renderShader);
	void SetMatrices(glm::mat4& projectionMat, glm::mat4 viewMat, glm::vec3 cameraDirection, glm::vec3 up);

private:

	Shader m_particleShader = Shader();
	Random random = Random();

	const int MAX_PARTICLES = 40;

	unsigned int m_VAOs[2];
	unsigned int m_VBOs[2];
	unsigned int m_transformFeedbackBuffer;
	unsigned int m_query;

	int m_currentReadBuffer = 0;

	float m_elapsedTime = 0;
	int m_currentNumberOfParticles = 0;

	glm::mat4 m_projectionMat;
	glm::mat4 m_viewMat;
	glm::vec3 m_quad1;
	glm::vec3 m_quad2;

	Material m_material;

	struct Particle
	{
		glm::vec3 Position;
		glm::vec3 Velocity;
		glm::vec3 Color;
		float Size = 1;
		float Type = 0;
	};

	enum ParticleType
	{
		GENERATOR_PARTICLE = 0,
		NORMAL_PARTICLE = 1,
	};
};

