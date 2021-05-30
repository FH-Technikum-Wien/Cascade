#include "World.h"

World::World(const Camera& camera, const Light& light, unsigned int screenWidth, unsigned int screenHeight) : m_camera(camera), m_light(light)
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	/// DISPLACEMENT (normal rendering)

	m_displacementShader.addShader(VERTEX_SHADER_DISPLACEMENT, ShaderType::VERTEX_SHADER);
	m_displacementShader.addShader(FRAGMENT_SHADER_DISPLACEMENT, ShaderType::FRAGMENT_SHADER);

	m_displacementShader.activate();
	m_displacementShader.setInt("diffuseTexture", 0);
	m_displacementShader.setInt("normalMap", 1);
	m_displacementShader.setInt("displacementMap", 2);
	m_displacementShader.setInt("shadowMap", 3);
	m_displacementShader.setMat4("projectionMat", camera.ProjectionMat);
	m_displacementShader.setFloat("ambientLightAmount", AmbientLight);

	m_displacementShader.setFloat("lightIntensity", m_light.intensity);
	m_displacementShader.setVec3("lightColor", m_light.color);
	m_displacementShader.setVec3("lightPos", m_light.position);

	/// SHADOWS

	m_depthShader.addShader(VERTEX_SHADER_SHADOW_GEN, ShaderType::VERTEX_SHADER);
	m_depthShader.addShader(FRAGMENT_SHADER_SHADOW_GEN, ShaderType::FRAGMENT_SHADER);

	m_depthShader.activate();
	// Framebuffer for rendering the depthMap.
	glGenFramebuffers(1, &m_depthMapFBO);
	// Create depthMap texture.
	glGenTextures(1, &m_depthMap);
	glBindTexture(GL_TEXTURE_2D, m_depthMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, m_shadowTextureWidth, m_shadowTextureHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Everything outside of light frustum has depth of 1.0 -> no shadow.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// Attach depth texture to FrameBufferObject's depth buffer.
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_depthMap, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	/// FILTERING

	m_filterShader.addShader(VERTEX_SHADER_GAUSSIAN, ShaderType::VERTEX_SHADER);
	m_filterShader.addShader(FRAGMENT_SHADER_GAUSSIAN, ShaderType::FRAGMENT_SHADER);
	m_filterShader.activate();
	m_filterShader.setInt("filterTexture", 0);

	// Framebuffer for rendering the depthMap.
	glGenFramebuffers(1, &m_filterFBO);
	// Create depthMap texture.
	glGenTextures(1, &m_filterMap);
	glBindTexture(GL_TEXTURE_2D, m_filterMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, m_shadowTextureWidth, m_shadowTextureHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// Attach depth texture to FrameBufferObject's depth buffer.
	glBindFramebuffer(GL_FRAMEBUFFER, m_filterFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_filterMap, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenVertexArrays(1, &m_filterVAO);
	glBindVertexArray(m_filterVAO);

	// Position
	glGenBuffers(1, &m_filterVBO_Vertices);
	glBindBuffer(GL_ARRAY_BUFFER, m_filterVBO_Vertices);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), &m_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// UVs
	glGenBuffers(1, &m_filterVBO_Uvs);
	glBindBuffer(GL_ARRAY_BUFFER, m_filterVBO_Uvs);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), &m_uvs, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);


	// TESSELATION
	m_tesselationShader.addShader(TESSELLATION_VERTEX_SHADER, ShaderType::VERTEX_SHADER);
	m_tesselationShader.addShader(TESSELLATION_CONTROL_SHADER, ShaderType::TESS_CONTROL_SHADER);
	m_tesselationShader.addShader(TESSELLATION_EVAL_SHADER, ShaderType::TESS_EVAL_SHADER);
	m_tesselationShader.addShader(TESSELLATION_FRAGMENT_SHADER, ShaderType::FRAGMENT_SHADER);

	m_tesselationShader.activate();
	m_tesselationShader.setInt("diffuseTexture", 0);
	m_tesselationShader.setInt("normalMap", 1);
	m_tesselationShader.setInt("displacementMap", 2);

	m_tesselationShader.setMat4("projectionMat", camera.ProjectionMat);
	m_tesselationShader.setFloat("ambientLightAmount", AmbientLight);

	m_tesselationShader.setFloat("lightIntensity", m_light.intensity);
	m_tesselationShader.setVec3("lightColor", m_light.color);
	m_tesselationShader.setVec3("lightPos", m_light.position);


	Material terrainMat = Material("art/terrain_displacement.jpg", "art/terrain_normal.jpg", "art/terrain_displacement.jpg", GL_RGB);
	terrainMat.ambientStrength = 0.5f;
	m_terrain = new Terrain(terrainMat, glm::vec3(-5.0f, 0.0f, 2.0f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(1.0f));
}

void World::Add(Object* object)
{
	m_objects.push_back(object);
}

void World::Render(bool wireframeMode)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Render depth of scene to depthMap texture
	m_depthShader.activate();
	m_depthShader.setMat4("lightSpaceMat", m_light.lightSpaceMat);
	glViewport(0, 0, m_shadowTextureWidth, m_shadowTextureHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);

	// Render world's depth
	for (Object* object : m_objects)
	{
		object->RenderDepth(m_depthShader);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Clear color buffer and depth buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	/// GAUSSIAN BLUR - Two way pass

	m_filterShader.activate();
	m_filterShader.setVec3("blurScale", glm::vec3(1.0f / (m_shadowTextureWidth * m_blurAmount), 0.0f, 0.0f));

	// Blur x-axis
	glBindFramebuffer(GL_FRAMEBUFFER, m_filterFBO);
	// Bind texture to apply blur to
	glBindTexture(GL_TEXTURE_2D, m_depthMap);

	glBindVertexArray(m_filterVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); 
	glBindVertexArray(0);

	// Blur y-axis (use blured texture)
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	m_filterShader.setVec3("blurScale", glm::vec3(0.0f, 1.0f / (m_shadowTextureHeight * m_blurAmount), 0.0f));
	// Bind texture to apply blur to
	glBindTexture(GL_TEXTURE_2D, m_filterMap);

	glBindVertexArray(m_filterVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Reset
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset viewport.
	glViewport(0, 0, m_screenWidth, m_screenHeight);

	m_displacementShader.activate();
	m_displacementShader.setMat4("viewMat", m_camera.GetViewMat());
	m_displacementShader.setVec3("cameraPos", m_camera.Position);
	m_displacementShader.setFloat("bumpiness", Input::Bumpiness);
	m_displacementShader.setFloat("heightScale", HeightScale);
	m_displacementShader.setInt("steps", Steps);
	m_displacementShader.setInt("refinementSteps", RefinementSteps);
	m_displacementShader.setMat4("lightSpaceMat", m_light.lightSpaceMat);

	m_displacementShader.setFloat("minVariance", MinVariance);

	// Bind depth texture
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_depthMap);

	for (Object* object : m_objects)
	{
		object->Render(m_displacementShader, wireframeMode);
	}

	m_tesselationShader.activate();
	m_tesselationShader.setMat4("viewMat", m_camera.GetViewMat());
	m_tesselationShader.setVec3("cameraPos", m_camera.Position);
	m_tesselationShader.setFloat("bumpiness", Input::Bumpiness);
	m_tesselationShader.setMat4("lightSpaceMat", m_light.lightSpaceMat);

	m_tesselationShader.setFloat("displacementFactor", TesselationDisplacementFactor);
	m_tesselationShader.setFloat("tesselationAmount", TesselationAmount);

	m_terrain->Render(m_tesselationShader, wireframeMode);
}

std::vector<float> World::GetWorldVertices()
{
	std::vector<float> vertices = std::vector<float>();
	for (Object* object : m_objects)
	{
		std::vector<float> objectVertices = object->GetVerticesInWorldSpace();
		vertices.insert(vertices.end(), objectVertices.begin(), objectVertices.end());
	}

	return vertices;
}
