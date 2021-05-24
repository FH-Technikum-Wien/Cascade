#pragma once
#include <glm/vec3.hpp>

#include "../objects/Material.h"
#include "../shaders/Shader.h"
#include "../util/Random.h"
#include "Camera.h"

class ParticleSystem
{
public:
	enum ParticleType
	{
		GENERATOR_PARTICLE = 0,
		NORMAL_PARTICLE = 1,
		COLOR_BLEND_ON_LIFETIME = 2,
		CONFETTI = 3,
	};

	glm::vec3 SpawnPosition = glm::vec3(3.0f, 0.0f, 0.0f);
	glm::vec3 VelocityMin = glm::vec3(-0.5f, 1, -0.5f);
	glm::vec3 VelocityRange = glm::vec3(1, 4, 1);
	glm::vec3 Gravity = glm::vec3(0, -0.25f, 0);
	glm::vec3 Color = glm::vec3(1.0f, 0.05f, 0.0f);
	glm::vec3 ColorBlendStart = glm::vec3(0.05f, 0.0f, 1.0f);
	glm::vec3 ColorBlendEnd = glm::vec3(0.0f, 1.0f, 0.0f);
	ParticleType ParticleTypeToSpawn = ParticleType::NORMAL_PARTICLE;
	float Size = 0.5f;
	float LifetimeMin = 1;
	float LifetimeRange = 3;
	float SpawnFrequence = 0.001f;
	int NumberOfParticlesToSpawn = 1;


public:
	ParticleSystem(const Camera& camera);

	void Update(const Camera& camera, float deltaTime);
	void Render(const Camera& camera, bool wireframeMode);
	void SetMatrices(const Camera& camera);

	int GetNumberOfParticles();
	int GetNumberOfGenerators();

private:
	const char* PARTICLE_UPDATE_VERTEX_SHADER = "src/shaders/particles/updating.vert";
	const char* PARTICLE_UPDATE_GEOMETRY_SHADER = "src/shaders/particles/updating.geom";

	const char* PARTICLE_RENDER_VERTEX_SHADER = "src/shaders/particles/rendering.vert";
	const char* PARTICLE_RENDER_GEOMETRY_SHADER = "src/shaders/particles/rendering.geom";
	const char* PARTICLE_RENDER_FRAGMENT_SHADER = "src/shaders/particles/rendering.frag";

	const int MAX_PARTICLES = 500000;
	const int SHADER_MAX_PARTICLES = 40;

	const char* BRICK_WALL_2 = "art/particle.png";

	Shader m_updateShader = Shader();
	Shader m_renderShader = Shader();

	Random random = Random();

	unsigned int m_VAOs[2];
	unsigned int m_VBOs[2];
	unsigned int m_transformFeedbackBuffer;
	unsigned int m_query;

	int m_currentReadBuffer = 0;

	float m_elapsedTime = 0;
	int m_currentNumberOfParticles = 0;
	int m_currentNumberOfGenerators = 0;

	glm::mat4 m_viewMat;
	glm::vec3 m_quad1;
	glm::vec3 m_quad2;

	Material m_material;

	class Particle
	{
	public:
		glm::vec3 Position;
		glm::vec3 Velocity;
		glm::vec3 Color;
		float Lifetime = 1;
		float Size = 1;
		float Type = 0;
	};
};

