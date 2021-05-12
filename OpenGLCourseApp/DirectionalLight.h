#pragma once
#include "Light.h"

class DirectionalLight:
	public Light
{
public:
	DirectionalLight();
	DirectionalLight(GLfloat shadowWidth, GLuint shadowHeight, GLfloat red, GLfloat blue, GLfloat green, GLfloat aIntensity, GLfloat dIntensity, GLfloat xDir, GLfloat yDir, GLfloat zDir);

	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColorLocation, GLfloat diffuseIntensityLocation, GLfloat directionLocation);

	glm::mat4 CalculateLightTransform();


	~DirectionalLight();

private:
	glm::vec3 direction;
};

