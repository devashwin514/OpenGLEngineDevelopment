#pragma once

#include <GL/glew.h>


class Material
{
public:
	Material();
	Material(GLfloat sIntensity, GLfloat shineValue);


	void UseMaterial(GLuint specularIntensityLocation, GLuint shineLocation);


	~Material();

private:
	GLfloat specularIntensity, shininess;
};

