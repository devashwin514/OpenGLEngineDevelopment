#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);

	void KeyControl(bool* keys, GLfloat deltaTime);
	void MouseControl(GLfloat xChange, GLfloat yChange);

	glm::vec3 getCameraPostion();
	glm::vec3 getCameraDirection();

	glm::mat4 CalculateViewMatrix();

	~Camera();

private:
	glm::vec3 position, front, up, right, worldUp;

	GLfloat yaw, pitch;

	GLfloat movementSpeed, turnSpeed;

	void update();
};

