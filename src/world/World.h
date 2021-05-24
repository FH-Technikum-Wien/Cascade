#pragma once
#include <vector>
#include "../objects/Object.h"

#include "Camera.h"
#include "Light.h"
#include "Input.h"


class World
{
public:
	World(const Camera& camera, const Light& light, unsigned int screenWidth, unsigned int screenHeight);

	void Add(Object* object);
	void Render(bool wireframeMode);

	std::vector<float> GetWorldVertices();

public:
	float HeightScale = 0.1f;
	float HeightScaleSteps = 0.05f;

	int Steps = 10;
	int RefinementSteps = 1;

	float AmbientLight = 0.2f;

private:
	std::vector<Object*> m_objects = std::vector<Object*>();

	const char* VERTEX_SHADER_DISPLACEMENT = "src/shaders/displacement/shader.vert";
	const char* FRAGMENT_SHADER_DISPLACEMENT = "src/shaders/displacement/shader.frag";

	const char* VERTEX_SHADER_SHADOW = "src/shaders/shadows/depthShader.vert";
	const char* FRAGMENT_SHADER_SHADOW = "src/shaders/shadows/depthShader.frag";

	const Camera& m_camera;
	const Light& m_light;

	Shader m_displacementShader = Shader();
	Shader m_depthShader = Shader();

	unsigned int m_depthMapFBO;
	unsigned int m_depthMap;

	unsigned int m_shadowTextureWidth = 1024;
	unsigned int m_shadowTextureHeight = 1024;

	unsigned int m_screenWidth = 0;
	unsigned int m_screenHeight = 0;
};

