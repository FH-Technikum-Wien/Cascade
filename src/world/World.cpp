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

	m_depthShader.addShader(VERTEX_SHADER_SHADOW, ShaderType::VERTEX_SHADER);
	m_depthShader.addShader(FRAGMENT_SHADER_SHADOW, ShaderType::FRAGMENT_SHADER);

	m_depthShader.activate();
	// Framebuffer for rendering the depthMap.
	glGenFramebuffers(1, &m_depthMapFBO);
	// Create depthMap texture.
	glGenTextures(1, &m_depthMap);
	glBindTexture(GL_TEXTURE_2D, m_depthMap);
	// Set resolution and only use 'DEPTH_COMPONENT', only need depth.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_shadowTextureWidth, m_shadowTextureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// Everything outside of light frustum has depth of 1.0 -> no shadow.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// Attach depth texture to FrameBufferObject's depth buffer.
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
	// Tell OpenGL that we don't need any color.
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void World::Add(Object* object)
{
	m_objects.push_back(object);
}

void World::Render(bool wireframeMode)
{
	// Render depth of scene to depthMap texture
	m_depthShader.activate();
	m_depthShader.setMat4("lightSpaceMat", m_light.lightSpaceMat);
	glViewport(0, 0, m_shadowTextureWidth, m_shadowTextureHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	// Prevent peter panning by using back-faces
	glCullFace(GL_FRONT);
	// Render world's depth
	for (Object* object : m_objects)
	{
		object->RenderDepth(m_depthShader);
	}
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Reset viewport.
	glViewport(0, 0, m_screenWidth, m_screenHeight);
	// Clear color buffer and depth buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_displacementShader.activate();
	m_displacementShader.setMat4("viewMat", m_camera.GetViewMat());
	m_displacementShader.setVec3("cameraPos", m_camera.Position);
	m_displacementShader.setFloat("bumpiness", Input::Bumpiness);
	m_displacementShader.setFloat("heightScale", HeightScale);
	m_displacementShader.setInt("steps", Steps);
	m_displacementShader.setInt("refinementSteps", RefinementSteps);
	m_displacementShader.setMat4("lightSpaceMat", m_light.lightSpaceMat);

	// Bind depth texture
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_depthMap);

	for (Object* object : m_objects)
	{
		object->Render(m_displacementShader, wireframeMode);
	}
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
