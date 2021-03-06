#include <stdexcept>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>

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
Light light = Light(glm::vec3(-15.0f, 15.0f, 6.0f), 1.0f);

bool wireframeModeActive = false;
bool showLightFrustum = false;

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


	Material material = Material("art/bricks2.jpg", "art/bricks2_normal.jpg", "art/bricks2_disp.jpg", GL_RGB);
	material.ambientStrength = 0.1f;
	material.diffuseStrength = 1.0f;
	material.specularStrength = 0.2f;
	material.focus = 32.0f;
	

	Material groundMat = Material("art/brickWall.jpg", "art/brickWall_normal.jpg", GL_RGB);

	// Ground
	world->Add(new Cube(groundMat, glm::vec3(0.0f, -3.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(15.0f, 0.5f, 15.f)));

	world->Add(new Cube(groundMat, glm::vec3(-3.0f, -1.5f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f)));

	world->Add(new Cube(groundMat, glm::vec3(-11.0f, -2.5f, -5.0f), glm::vec3(0.0f, 45.0f, 0.0f), glm::vec3(0.5f)));

	world->Add(new Cube(material, glm::vec3(-8.0f, -1.5f, -7.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f)));

	world->Add(new Cube(material, glm::vec3(-10.0f, 5.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f)));

	world->Add(new Cube(material, glm::vec3(7.0f, -1.0f, -6.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f)));

	world->Add(new Plane(material, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f)));

	setupKdTree();

	std::chrono::high_resolution_clock clock;
	auto lastFrameTime = clock.now();

	float timePressed = 0;
	float delay = 0.1f;

	float titleLastUpdate = 0;
	float titleUpdateDelay = 0.2f;

	double deltaTimeSum = 0;
	int frames = 0;
	int FPS = 0;

	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltaTime in seconds
		auto currentFrameTime = clock.now();
		double deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(currentFrameTime - lastFrameTime).count() / 1e9;
		lastFrameTime = currentFrameTime;

		// Calculate FPS
		deltaTimeSum += deltaTime;
		++frames;

		// Update every second
		if (deltaTimeSum > 1.0f)
		{
			FPS = frames;
			frames = 0;
			deltaTimeSum = 0;
		}


		printError();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		// Sets one color for window (background)
		glClearColor(0.2f, 0.4f, 0.4f, 1.0f);
		// Clear color buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		world->Render(wireframeModeActive);

		proceduralSystem->Update(camera, wireframeModeActive);

		particleSystem->Update(camera, deltaTime);
		particleSystem->Render(camera, wireframeModeActive);

		// Handle iput
		Input::ProcessContinuousInput(window, &camera);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && timePressed + delay < glfwGetTime())
		{
			timePressed = glfwGetTime();
			HandleMouseClick();
		}

		if (titleLastUpdate + titleUpdateDelay < glfwGetTime())
		{
			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << Input::Bumpiness;
			std::string bumpiness = stream.str();
			stream.str(std::string());

			stream << std::fixed << std::setprecision(2) << world->HeightScale;
			std::string heightScale = stream.str();
			stream.str(std::string());

			stream << std::fixed << std::setprecision(6) << world->MinVariance;
			std::string variance = stream.str();
			stream.str(std::string());

			stream << std::fixed << std::setprecision(3) << particleSystem->SpawnFrequence;
			std::string spawnFrequency = stream.str();
			stream.str(std::string());

			stream << std::fixed << std::setprecision(3) << world->TesselationAmount;
			std::string tessAmount = stream.str();
			stream.str(std::string());

			stream << std::fixed << std::setprecision(3) << world->TesselationDisplacementFactor;
			std::string tessDisplacement = stream.str();
			stream.str(std::string());


			titleLastUpdate = glfwGetTime();
			std::string lastInput =
				"FPS: " + std::to_string(FPS) +
				" | Bumpiness(Wheel): " + bumpiness +
				" | HeightScale(Q,E): " + heightScale +
				" | Steps(Right,Left): " + std::to_string(world->Steps) +
				" | R-Steps(Up,Down): " + std::to_string(world->RefinementSteps) +
				" | MinVariance(v,b): " + variance +
				" | P_Mode(Num[1,2,3]): " + std::to_string(particleSystem->ParticleTypeToSpawn) +
				" | P_To_Spawn(*,/): " + std::to_string(particleSystem->NumberOfParticlesToSpawn) +
				" | P_Frequency(+,-): " + spawnFrequency +
				" | P_Number: " + std::to_string(particleSystem->GetNumberOfParticles()) +
				" | PG_Number: " + std::to_string(particleSystem->GetNumberOfGenerators());
				//" | Tess_Level: " + tessAmount +
				//" | Tess_Displ: " + tessDisplacement;
			glfwSetWindowTitle(window, lastInput.c_str());
		}

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
	// Tell GLFW that we're using OpenGL version 4.6
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

	GLint maxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &maxPatchVertices);
	std::cout << "Maximum supported patch vertices: " << maxPatchVertices << std::endl;
	// Size of input patch for tesselation is 3
	glPatchParameteri(GL_PATCH_VERTICES, 3);

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
	{
		if (particleSystem->NumberOfParticlesToSpawn == 0)
			++particleSystem->NumberOfParticlesToSpawn;
		else
			particleSystem->NumberOfParticlesToSpawn *= 2;
	}
	if (key == GLFW_KEY_KP_DIVIDE && action == GLFW_PRESS)
		particleSystem->NumberOfParticlesToSpawn /= 2;

	if (key == GLFW_KEY_V && action == GLFW_PRESS)
		world->MinVariance *= 2;
	if (key == GLFW_KEY_B && action == GLFW_PRESS)
		world->MinVariance /= 2;

	if (key == GLFW_KEY_KP_8 && action == GLFW_PRESS)
		world->TesselationDisplacementFactor *= 2;
	if (key == GLFW_KEY_KP_2 && action == GLFW_PRESS)
		world->TesselationDisplacementFactor /= 2;

	if (key == GLFW_KEY_KP_6 && action == GLFW_PRESS)
		world->TesselationAmount *= 2;
	if (key == GLFW_KEY_KP_4 && action == GLFW_PRESS)
		world->TesselationAmount /= 2;

	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		showLightFrustum = !showLightFrustum;
		world->ShowLightFrustum(showLightFrustum);
	}
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