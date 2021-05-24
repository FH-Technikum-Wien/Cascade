#include <stdexcept>
#include <vector>
#include <chrono>

#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "objects/Material.h"
#include "objects/Plane.h"
#include "objects/Cube.h"
#include "world/Camera.h"
#include "world/Input.h"
#include "world/Chunk.h"
#include "world/Light.h"

#include "world/ProceduralSystem.h"
#include "world/ParticleSystem.h"
#include "intersection/KdTree.h"
#include "world/World.h"

// Defines the glfw window size
#define SCREEN_WIDTH 1920.0f
#define SCREEN_HEIGHT 1080.0f

GLFWwindow* window = nullptr;
Camera camera = Camera(SCREEN_WIDTH, SCREEN_HEIGHT, glm::vec3(0.0f, 0.0f, 3.0f), 0.1f);
Light light = Light(glm::vec3(-1.0f, 5.0f, 0.2f), 1.0f);

bool wireframeModeActive = false;

World* world;

ProceduralSystem* proceduralSystem;
ParticleSystem* particleSystem;

KdTree* kdTree;

void setupGLFW();
void setupKdTree();

void HandleMouseClick();

void mousePositionCallback(GLFWwindow* window, double xPos, double yPos);
void keyPressedCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void printError();


int main()
{
	setupGLFW();

	world = new World(camera, light, SCREEN_WIDTH, SCREEN_HEIGHT);
	proceduralSystem = new ProceduralSystem(SCREEN_WIDTH, SCREEN_HEIGHT, camera);
	particleSystem = new ParticleSystem(camera);

	Material groundMat = Material("art/brickWall.jpg", "art/brickWall_normal.jpg", GL_RGB);
	world->Add(new Cube(groundMat, glm::vec3(0.0f, -4.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 1.0f, 10.f)));

	Material material = Material("art/bricks2.jpg", "art/bricks2_normal.jpg", "art/bricks2_disp.jpg", GL_RGB);
	material.ambientStrength = 0.1f;
	material.diffuseStrength = 1.0f;
	material.specularStrength = 0.2f;
	material.focus = 32.0f;
	world->Add(new Plane(material, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f)));

	world->Add(new Cube(material, glm::vec3(-5.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f)));

	setupKdTree();

	std::chrono::high_resolution_clock clock;
	auto lastFrameTime = clock.now();

	float timePressed = 0;
	float delay = 0.1f;

	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltaTime in seconds
		auto currentFrameTime = clock.now();
		double deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(currentFrameTime - lastFrameTime).count() / 1e9;
		lastFrameTime = currentFrameTime;

		printError();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		// Sets one color for window (background)
		glClearColor(0.2f, 0.4f, 0.4f, 1.0f);
		// Clear color buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		world->Render(wireframeModeActive);

		proceduralSystem->Update(camera);

		particleSystem->Update(camera, deltaTime);
		particleSystem->Render(camera);

		// Handle iput
		Input::ProcessContinuousInput(window, &camera);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && timePressed + delay < glfwGetTime())
		{
			timePressed = glfwGetTime();
			HandleMouseClick();
		}


		std::string lastInput =
			"Bumpiness: " + std::to_string(Input::Bumpiness) +
			" | HeightScale: " + std::to_string(world->HeightScale) +
			" | Steps: " + std::to_string(world->Steps) +
			" | Refinement Steps: " + std::to_string(world->RefinementSteps) +
			" | Particle Mode: " + std::to_string(particleSystem->ParticleTypeToSpawn) +
			" | Particle to spawn: " + std::to_string(particleSystem->NumberOfParticlesToSpawn) +
			" | Particle spawn frequency: " + std::to_string(particleSystem->SpawnFrequence);
		glfwSetWindowTitle(window, lastInput.c_str());

		// Swaps the drawn buffer with the buffer that got written to
		glfwSwapBuffers(window);
		// Checks if any events are triggered and executes callbacks
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

void setupGLFW()
{
	glfwInit();
	// Tell GLFW that we're using OpenGL version 4.5
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	// Tell GLFW to use Core_Profile -> Smaller subset without backwards-compatibility (not needed)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "", nullptr, nullptr);
	if (window == nullptr)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create the GLFW-Window");
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::runtime_error("Failed to initialize GLAD");
	}

	// Set OpenGL viewport
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST);
	// Events
	glfwSetCursorPosCallback(window, mousePositionCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
	glfwSetKeyCallback(window, keyPressedCallback);
	glfwSetScrollCallback(window, Input::ProcessScrollInput);
	// Lock cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	std::cout << glGetString(GL_VERSION) << std::endl;
}

void setupKdTree()
{
	// Kd-Tree
	std::vector<float> vertices = world->GetWorldVertices();

	std::cout << "\n[*] Building kd-tree" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	kdTree = new KdTree(&vertices[0], vertices.size() / 3);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "[->] Done!" << std::endl;
	std::cout << "Building time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds." << std::endl;
	kdTree->printStatistics();
}

void HandleMouseClick()
{
	KdStructs::Vector position = KdStructs::Vector(camera.Position.x, camera.Position.y, camera.Position.z);
	glm::vec3 directionVector = camera.Orientation * glm::vec3(0, 0, -1);
	// Normalize
	directionVector = glm::normalize(directionVector);
	KdStructs::Vector direction = KdStructs::Vector(directionVector.x, directionVector.y, directionVector.z);

	// Cast ray into scene
	KdStructs::RayHit* hit = nullptr;

	std::cout << "\n[*] Casting Ray." << std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	kdTree->raycast(KdStructs::Ray(position, direction, 1000), hit);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Raycast time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds." << std::endl;
	std::cout << std::endl;

	if (hit != nullptr)
		particleSystem->SpawnPosition = glm::vec3(hit->position[0], hit->position[1], hit->position[2]);
}

#pragma region Input
void mousePositionCallback(GLFWwindow* window, double xPos, double yPos)
{
	camera.ProcessMouse(xPos, yPos);
}

void keyPressedCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
		wireframeModeActive = !wireframeModeActive;

	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		world->HeightScale -= world->HeightScaleSteps;

	if (key == GLFW_KEY_E && action == GLFW_PRESS)
		world->HeightScale += world->HeightScaleSteps;

	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		++world->Steps;
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		--world->Steps;
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		++world->RefinementSteps;
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		--world->RefinementSteps;

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		camera.MovementSpeed = 0.01f;
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		camera.MovementSpeed = 0.1f;
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		camera.MovementSpeed = 1.0f;
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		camera.MovementSpeed = 10.0f;

	if (key == GLFW_KEY_KP_1 && action == GLFW_PRESS)
		particleSystem->ParticleTypeToSpawn = ParticleSystem::ParticleType::NORMAL_PARTICLE;
	if (key == GLFW_KEY_KP_2 && action == GLFW_PRESS)
		particleSystem->ParticleTypeToSpawn = ParticleSystem::ParticleType::COLOR_BLEND_ON_LIFETIME;
	if (key == GLFW_KEY_KP_3 && action == GLFW_PRESS)
		particleSystem->ParticleTypeToSpawn = ParticleSystem::ParticleType::CONFETTI;

	if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS)
		particleSystem->SpawnFrequence *= 2;
	if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS)
		particleSystem->SpawnFrequence /= 2;

	if (key == GLFW_KEY_KP_MULTIPLY && action == GLFW_PRESS)
		particleSystem->NumberOfParticlesToSpawn *= 2;
	if (key == GLFW_KEY_KP_DIVIDE && action == GLFW_PRESS)
		particleSystem->NumberOfParticlesToSpawn /= 2;
}
#pragma endregion


void printError()
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cout << "Error: " << error << std::endl;
	}
}