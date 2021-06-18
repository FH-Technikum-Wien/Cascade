#pragma once
#include <vector>
#include "../objects/Object.h"

#include "Camera.h"
#include "Light.h"
#include "Input.h"
#include "../objects/Terrain.h"
#include "../objects/Plane.h"


class World
{
public:
	World(const Camera& camera, const Light& light, unsigned int screenWidth, unsigned int screenHeight);

	void Add(Object* object);
	void Render(bool wireframeMode);
	void ShowLightFrustum(bool show);

	std::vector<float> GetWorldVertices();

public:
	float HeightScale = 0.1f;
	float HeightScaleSteps = 0.05f;

	int Steps = 10;
	int RefinementSteps = 1;

	float AmbientLight = 0.2f;

	float MinVariance = 0.0001f;

	float TesselationDisplacementFactor = 1.0f;
	float TesselationAmount = 1.0f;

private:
	std::vector<Object*> m_objects = std::vector<Object*>();

	const char* VERTEX_SHADER_DISPLACEMENT = "src/shaders/displacement/shader.vert";
	const char* FRAGMENT_SHADER_DISPLACEMENT = "src/shaders/displacement/shader.frag";

	const char* VERTEX_SHADER_SHADOW_GEN = "src/shaders/shadows/VSM/generator.vert";
	const char* FRAGMENT_SHADER_SHADOW_GEN = "src/shaders/shadows/VSM/generator.frag";

	const char* VERTEX_SHADER_GAUSSIAN = "src/shaders/filtering/gaussian.vert";
	const char* FRAGMENT_SHADER_GAUSSIAN = "src/shaders/filtering/gaussian.frag";

	const char* TESSELLATION_VERTEX_SHADER = "src/shaders/tessellation/shader.vert";
	const char* TESSELLATION_CONTROL_SHADER = "src/shaders/tessellation/shader.tesc";
	const char* TESSELLATION_EVAL_SHADER = "src/shaders/tessellation/shader.tese";
	const char* TESSELLATION_FRAGMENT_SHADER = "src/shaders/tessellation/shader.frag";

	const Camera& m_camera;
	const Light& m_light;

	Shader m_displacementShader = Shader();
	Shader m_depthShader = Shader();

	Shader m_filterShader = Shader();
	Plane filterPlane = Plane(Material(), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f));

	Shader m_tesselationShader = Shader();
	Terrain* m_terrain;
	Terrain* m_terrain2;

	unsigned int m_depthMapFBO;
	unsigned int m_depthMap;

	unsigned int m_filterVAO;
	unsigned int m_filterVBO_Vertices;
	unsigned int m_filterVBO_Uvs;
	unsigned int m_filterFBO;
	unsigned int m_filterMap;

	float m_vertices[12] =
	{
		-1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		
		
		
	};

	float m_uvs[8] =
	{
		 0.0f, 1.0f,
		 0.0f, 0.0f,
		 1.0f, 1.0f,
		 1.0f, 0.0f,
	};

	unsigned int m_shadowTextureWidth = 1024;
	unsigned int m_shadowTextureHeight = 1024;

	unsigned int m_screenWidth = 0;
	unsigned int m_screenHeight = 0;

	float m_blurAmount = 0.5f;
};

