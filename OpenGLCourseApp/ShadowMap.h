#pragma once

#include <stdio.h>

#include <GL/glew.h>


class ShadowMap
{
public:
	ShadowMap();


	virtual bool init(GLuint width, GLuint height);

	virtual void Write();

	virtual void Read(GLenum textureUnit);

	GLuint GetShadowWidth() { return shadowWidth; }
	GLuint GetShadowHeight() { return shadowHeight; }


	~ShadowMap();

protected:
	GLuint FBO, shadowMap, shadowWidth, shadowHeight;
};

