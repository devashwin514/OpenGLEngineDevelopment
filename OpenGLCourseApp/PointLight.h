#pragma once
#include "Light.h"
#include <vector>


class PointLight :
	public Light
{
public:
	PointLight();
	PointLight(GLfloat red, GLfloat green, GLfloat blue, 
			GLfloat aIntensity, GLfloat dIntensity, 
			GLfloat xPos, GLfloat yPos, GLfloat zPos, 
			GLfloat con, GLfloat lin, GLfloat exp);

	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColorLocation, GLfloat diffuseIntensityLocation, GLfloat positionLocation, GLfloat constantLocation, GLfloat linearLocation, GLfloat expLocation);

	std::vector<glm::mat4> CalculateLightTransform();

	GLfloat GetFarPlane() { return farPlane; }
	glm::vec3 GetPosition() { return position; }

	~PointLight();

protected:
	glm::vec3 position;

	GLfloat constant, linear, exponent, farPlane;
};

