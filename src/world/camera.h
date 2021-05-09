#pragma once

#include <glm\vec3.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\transform.hpp>


enum class CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera
{
public:

	explicit Camera(float screenWidth, float screenHeight, glm::vec3 position, float movementSpeed) : Position(position), MovementSpeed(movementSpeed)
	{
		ProjectionMat = glm::perspective(glm::radians(45.0f), screenWidth / screenHeight, 0.1f, 1000.0f);
	}

	glm::mat4 GetViewMat() const
	{
		glm::mat4 lookAt = glm::lookAt(Position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		return glm::inverse(glm::translate(transform, Position) * glm::mat4_cast(Orientation));
	}

	void ProcessMouse(double xPos, double yPos)
	{
		xOffset += (lastMousePos.x - (float)xPos) * MouseSensitivity;
		yOffset += (lastMousePos.y - (float)yPos) * MouseSensitivity;
		lastMousePos = glm::vec2(xPos, yPos);

		Orientation = glm::quat(glm::vec3(yOffset, xOffset, 0.0f));
	}

	void ProcessInput(CameraMovement movement)
	{
		const float movementSpeed = MovementSpeed;

		switch (movement)
		{
		case CameraMovement::FORWARD:
			Position += Orientation * glm::vec3(0.0f, 0.0f, -1.0f) * movementSpeed;
			break;
		case CameraMovement::BACKWARD:
			Position -= Orientation * glm::vec3(0.0f, 0.0f, -1.0f) * movementSpeed;
			break;
		case CameraMovement::LEFT:
			Position -= Orientation * glm::vec3(1.0f, 0.0f, 0.0f) * movementSpeed;
			break;
		case CameraMovement::RIGHT:
			Position += Orientation * glm::vec3(1.0f, 0.0f, 0.0f) * movementSpeed;
			break;
		case CameraMovement::UP:
			Position += glm::vec3(0.0f, 1.0f, 0.0f) * movementSpeed;
			break;
		case CameraMovement::DOWN:
			Position -= glm::vec3(0.0f, 1.0f, 0.0f) * movementSpeed;
			break;
		}
	}

public:
	glm::mat4 ProjectionMat = glm::mat4(1);
	glm::vec3 Position = glm::vec3(0.0f);
	glm::quat Orientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
	float MovementSpeed = 1.0f;
	float MouseSensitivity = 0.001f;

private:
	glm::mat4 transform = glm::mat4(1.0f);
	glm::vec2 lastMousePos = glm::vec2(0.0f);

	float xOffset = 0.0f;
	float yOffset = 0.0f;
};