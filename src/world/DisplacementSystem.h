#pragma once
#include "../objects/Plane.h"
#include "Camera.h"
#include "Light.h"

class DisplacementSystem
{
public:
	DisplacementSystem(const Camera& camera, const Light& light);
	void Update(const Camera& camera, bool wireframeMode);

public:
	float HeightScale = 0.1f;
	float HeightScaleSteps = 0.05f;

	int Steps = 10;
	int RefinementSteps = 1;

	Plane plane;
	Shader shader = Shader();

private:
	const char* VERTEX_SHADER_DISPLACEMENT = "src/shaders/displacement/shader.vert";
	const char* FRAGMENT_SHADER_DISPLACEMENT = "src/shaders/displacement/shader.frag";

	const char* BRICK_WALL_2 = "art/bricks2.jpg";
	const char* BRICK_WALL_2_NORMAL = "art/bricks2_normal.jpg";
	const char* BRICK_WALL_2_DISPLACEMENT = "art/bricks2_disp.jpg";

	
};

