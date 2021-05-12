#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "CommonValues.h"

class Shader
{
public:
	Shader();

	void CreateFromString(const char* vertexCode, const char* fragmentCode);
	void CreateFromFiles(const char* vertexLocation, const char* fragmentLocation);
	void CreateFromFiles(const char* vertexLocation, const char* geomLocation, const char* fragmentLocation);

	std::string ReadFile(const char* fileLocation);

	GLuint GetProjectionLocation();
	GLuint GetModelLocation();
	GLuint GetViewLocation();
	GLfloat GetAmbientIntensityLocation();
	GLfloat GetAmbientColorLocation();
	GLuint GetDiffuseIntensityLocation();
	GLuint GetDirectionLocation();
	GLuint GetSpecularIntensityLocation();
	GLuint GetShininessLocation();
	GLuint GetEyePositionLocation();
	GLuint GetOmniLightPosLocation();
	GLuint GetFarPlaneLocation();

	void SetDirectionalLight(DirectionalLight *dLight);
	void SetPointLights(PointLight *pLight, unsigned int lightCount);
	void SetSpotLights(SpotLight *sLight, unsigned int lightCount);
	void SetTexture(GLuint texUnit);
	void SetDirShadowMap(GLuint texUnit);
	void SetDirLightTransform(glm::mat4 *ltransform);
	void SetLightMatrices(std::vector<glm::mat4> lightMat);

	void UseShader();
	void ClearShader();

	~Shader();

private:
	int pointLightCount;
	int spotLightCount;

	GLuint shaderID, uniformProjection, uniformModel, uniformView, uniformEyePostion, uniformSpecularIntensity, uniformShininess, uniformPointLightCount, uniformSpotLightCount, uniformDirLightTransform, uniformDirShadowMap, uniformTexture;

	GLuint uniformLightPos, uniformFarPlane;

	GLuint uniformLightMatrices[6];

	struct {
		GLuint uniformColor, uniformAmbientIntensity, uniformDiffuseIntensity, uniformDirection;
	} uniformDirectionalLight;

	struct {
		GLuint uniformColor, uniformAmbientIntensity, uniformDiffuseIntensity, uniformPosition, uniformConstant, uniformLinear, uniformExp;
	} uniformPointLight[MAX_POINT_LIGHTS];

	struct {
		GLuint uniformColor, uniformAmbientIntensity, uniformDiffuseIntensity, uniformPosition, uniformConstant, uniformLinear, uniformExp, uniformDirection, uniformEdge;
	} uniformSpotLight[MAX_SPOT_LIGHTS];

	void CompileShader(const char* vertexCode, const char* fragmentCode);
	void CompileShader(const char* vertexCode, const char* geomCode, const char* fragmentCode);
	void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);

	void CompileProgram();
};

