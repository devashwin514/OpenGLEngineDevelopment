#pragma once
#include "PointLight.h"


class SpotLight :
	public PointLight
{
public:
	SpotLight();
	SpotLight(GLfloat red, GLfloat green, GLfloat blue, 
			GLfloat aIntensity, GLfloat dIntensity, 
			GLfloat xPos, GLfloat yPos, GLfloat zPos,
			GLfloat xDir, GLfloat yDir, GLfloat zDir,
			GLfloat con, GLfloat lin, GLfloat exp,
			GLfloat edg);

	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColorLocation, 
				GLfloat diffuseIntensityLocation, GLfloat positionLocation, GLfloat directionLocation, 
				GLfloat constantLocation, GLfloat linearLocation, GLfloat expLocation, GLfloat edgeLocation);

	void SetFlashLight(glm::vec3 pos, glm::vec3 dir);


	~SpotLight();

private:
	glm::vec3 direction;

	GLfloat edge, procEdge;
};

