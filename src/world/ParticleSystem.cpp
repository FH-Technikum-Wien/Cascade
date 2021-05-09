#include "ParticleSystem.h"

void printError2()
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cout << "Error: " << error << std::endl;
	}
}

ParticleSystem::ParticleSystem(const Camera& camera)
{
	m_material = Material(BRICK_WALL_2, GL_RGB);

	m_updateShader.addShader(PARTICLE_UPDATE_VERTEX_SHADER, ShaderType::VERTEX_SHADER, false);
	m_updateShader.addShader(PARTICLE_UPDATE_GEOMETRY_SHADER, ShaderType::GEOMETRY_SHADER, false);

	// Define which attributes should be recorded by the transform feedback
	const char* attributes[6] = { "pPositionOut", "pVelocityOut", "pColorOut", "pLifetimeOut", "pSizeOut", "pTypeOut",
	};

	for (int i = 0; i < 6; i++)
	{
		glTransformFeedbackVaryings(m_updateShader.shaderProgramID, 6, attributes, GL_INTERLEAVED_ATTRIBS);
	}

	m_updateShader.linkProgram();

	m_renderShader.addShader(PARTICLE_RENDER_VERTEX_SHADER, ShaderType::VERTEX_SHADER, false);
	m_renderShader.addShader(PARTICLE_RENDER_GEOMETRY_SHADER, ShaderType::GEOMETRY_SHADER, false);
	m_renderShader.addShader(PARTICLE_RENDER_FRAGMENT_SHADER, ShaderType::FRAGMENT_SHADER, false);
	m_renderShader.linkProgram();

	glGenTransformFeedbacks(1, &m_transformFeedbackBuffer);
	glGenQueries(1, &m_query);

	glGenVertexArrays(2, m_VAOs);
	glGenBuffers(2, m_VBOs);

	Particle particle;
	particle.Type = ParticleType::GENERATOR_PARTICLE;

	for (int i = 0; i < 2; i++)
	{
		glBindVertexArray(m_VAOs[i]);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBOs[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * MAX_PARTICLES, nullptr, GL_DYNAMIC_DRAW);

		// Add generator particle to both buffers
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Particle), &particle);

		// Position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)0);
		glEnableVertexAttribArray(0);
		// Velocity
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)12);
		glEnableVertexAttribArray(1);
		// Color
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)24);
		glEnableVertexAttribArray(2);
		// Lifetime
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)36);
		glEnableVertexAttribArray(3);
		// Size
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)40);
		glEnableVertexAttribArray(4);
		// IsParticle
		glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)44);
		glEnableVertexAttribArray(5);
	}

	m_currentReadBuffer = 0;
	m_currentNumberOfParticles = 1;

	// Set non-changing generation data
	m_renderShader.activate();
	m_renderShader.setMat4("projectionMat", camera.ProjectionMat);
}

void ParticleSystem::Update(const Camera& camera, float deltaTime)
{
	m_updateShader.activate();
	m_updateShader.setVec3("gPosition", SpawnPosition);
	m_updateShader.setVec3("gVelocityMin", VelocityMin);
	m_updateShader.setVec3("gVelocityRange", VelocityRange);
	m_updateShader.setVec3("gGravity", Gravity);
	//m_updateShader.setVec3("gColor", Color);
	m_updateShader.setFloat("gSize", Size);
	m_updateShader.setFloat("gLifetimeMin", LifetimeMin);
	m_updateShader.setFloat("gLifetimeRange", LifetimeRange);
	m_updateShader.setFloat("sTimePassed", deltaTime);
	m_updateShader.setInt("gNumberOfParticlesToSpawn", 0);

	m_elapsedTime += deltaTime;

	// Spawn in defined time steps
	if (m_elapsedTime > SpawnFrequence)
	{
		m_elapsedTime -= SpawnFrequence;
		m_updateShader.setInt("gNumberOfParticlesToSpawn", NumberOfParticlesToSpawn);
		glm::vec3 randomSeed = glm::vec3(random.Xorshf96_01() * 30 - 10, random.Xorshf96_01() * 30 - 10, random.Xorshf96_01() * 30 - 10);
		m_updateShader.setVec3("gRandomSeed", randomSeed);
	}

	// Disable graphical output. We only want to update the particles, not render them
	glEnable(GL_RASTERIZER_DISCARD);
	// Tell opengl to use the transform feedback buffer
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedbackBuffer);

	// Bind current read-buffer
	glBindVertexArray(m_VAOs[m_currentReadBuffer]);
	// Enable velocity for calculation
	glEnableVertexAttribArray(1);

	// Tell opengl where to store the result of the tranform feedback
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_VBOs[1 - m_currentReadBuffer]);

	// Counts the number of outputted primitives
	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, m_query);

	// Start transform feedback rendering
	glBeginTransformFeedback(GL_POINTS);

	// Draw points
	glDrawArrays(GL_POINTS, 0, m_currentNumberOfParticles);

	// End transform feedback rendering
	glEndTransformFeedback();

	// End query
	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

	// Get result of query and save it in m_currentNumberOfParticles
	glGetQueryObjectiv(m_query, GL_QUERY_RESULT, &m_currentNumberOfParticles);

	// Swap read and write buffers for next iteration
	m_currentReadBuffer = 1 - m_currentReadBuffer;

	// Unbind transform feedback
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glDisable(GL_RASTERIZER_DISCARD);

	//std::cout << "Particle count: " << m_currentNumberOfParticles << std::endl;
}

void ParticleSystem::Render(const Camera& camera)
{
	SetMatrices(camera);

	//glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	// Disable writing to depth buffer, particles should not overwrite depth
	glDepthMask(0);

	// Add texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_material.texture);

	m_renderShader.activate();
	m_renderShader.setMat4("viewMat", m_viewMat);
	//m_renderShader.setVec3("cameraPos", camera.Position);
	m_renderShader.setVec3("quad1", m_quad1);
	m_renderShader.setVec3("quad2", m_quad2);

	// Render current read buffer (which we just wrote to)
	glBindVertexArray(m_VAOs[m_currentReadBuffer]);
	// Disable velocity, not needed for rendering
	glDisableVertexAttribArray(1);

	glDrawArrays(GL_POINTS, 0, m_currentNumberOfParticles);

	glDepthMask(1);
	glDisable(GL_BLEND);
}

void ParticleSystem::SetMatrices(const Camera& camera)
{
	m_viewMat = camera.GetViewMat();
	glm::vec3 forward = glm::mat4_cast(camera.Orientation) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	glm::vec3 up = glm::mat4_cast(camera.Orientation) * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	m_quad1 = glm::normalize(glm::cross(forward, up));
	m_quad2 = glm::normalize(glm::cross(forward, m_quad1));

}