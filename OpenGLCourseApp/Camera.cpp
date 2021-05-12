#include "Camera.h"



Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	movementSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	update();
}

void Camera::KeyControl(bool* keys, GLfloat deltaTime) {
	GLfloat velocity = movementSpeed * deltaTime;

	// Move forward when 'W' is pressed
	if (keys[GLFW_KEY_W]) {
		position += front * velocity;
	}

	// Move backwards when 'S' is pressed
	if (keys[GLFW_KEY_S]) {
		position -= front * velocity;
	}

	// Move to the left when 'A' is pressed
	if (keys[GLFW_KEY_A]) {
		position -= right * velocity;
	}

	// Move to the right when 'D' is pressed 
	if (keys[GLFW_KEY_D]) {
		position += right * velocity;
	}

	// Reset camera with 'R' key
	if (keys[GLFW_KEY_R]) {
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		front = glm::vec3(0.0f, 0.0f, -1.0f);
		yaw = -90.0f;
		pitch = 0.0f;
	}
}

void Camera::MouseControl(GLfloat xChange, GLfloat yChange) {
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f) {
		pitch = 89.0f;
	}

	if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	update();
}

glm::vec3 Camera::getCameraPostion() {
	return position;
}

glm::vec3 Camera::getCameraDirection()
{
	return glm::normalize(front);
}

glm::mat4 Camera::CalculateViewMatrix() {
	return glm::lookAt(position, position + front, worldUp);
}

void Camera::update() {
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, worldUp));
}

Camera::~Camera()
{
}
