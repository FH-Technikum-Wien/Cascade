#include "DisplacementSystem.h"
#include "../objects/Material.h"
#include "input.h"

DisplacementSystem::DisplacementSystem(const Camera& camera, const Light& light)
{
	Material material = Material(BRICK_WALL_2, BRICK_WALL_2_NORMAL, BRICK_WALL_2_DISPLACEMENT, GL_RGB);
	material.ambientStrength = 0.1f;
	material.diffuseStrength = 1.0f;
	material.specularStrength = 0.2f;
	material.focus = 32.0f;
	plane = Plane(material, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));

	shader.addShader(VERTEX_SHADER_DISPLACEMENT, ShaderType::VERTEX_SHADER);
	shader.addShader(FRAGMENT_SHADER_DISPLACEMENT, ShaderType::FRAGMENT_SHADER);

	shader.activate();
	shader.setInt("diffuseTexture", 0);
	shader.setInt("normalMap", 1);
	shader.setInt("displacementMap", 2);
	shader.setMat4("projectionMat", camera.ProjectionMat);
	shader.setFloat("ambientLightAmount", 1.0f);

	light.activateLight(shader);
}

void DisplacementSystem::Update(const Camera& camera, bool wireframeMode)
{
	shader.activate();
	shader.setMat4("viewMat", camera.GetViewMat());
	shader.setVec3("cameraPos", camera.Position);
	shader.setFloat("bumpiness", Input::Bumpiness);
	shader.setFloat("heightScale", HeightScale);
	shader.setInt("steps", Steps);
	shader.setInt("refinementSteps", RefinementSteps);

	plane.Render(shader, wireframeMode);
}
