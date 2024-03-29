#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION
#include "CommonValues.h"

// C++ Libs
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cmath>
#include <vector>

// OGL Libs
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM Libs
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Header Files for pipeline stages
#include "Mesh.h"
#include "Shader.h"
#include "MyWindow.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "Model.h"

#include <assimp/Importer.hpp>


MyWindow mainWindow;

std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Shader DirShadowShader; //OmniShadowShader;

Camera camera;

Texture brickTexture, dirtTexture, plainTexture;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

Material shinyMaterial, dullMaterial;

Model xWing, blackHawk;

GLfloat deltaTime = 0.0f, lastTime = 0.0f, blackHawkAngle = 0.0f;

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0, uniformDirLightTransform = 0, uniformOmniLightPos = 0, uniformFarPlane = 0;

unsigned int pointLightCount = 0, spotLightCount = 0;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.005f;

// Function to get offset variable for moving side-2-side in translation
float getTriOffset() {
	while (!mainWindow.getShouldClose()) {
		if (direction) {
			triOffset += triIncrement;
		}

		else if (!direction) {
			triOffset -= triIncrement;
		}

		if (abs(triOffset) >= triMaxOffset) {
			direction = !direction;
		}

		return triOffset;
	}
}

// Function to get the current angle for rotation
float curAngle = 0.0f;
float getCurAngle() {
	while (!mainWindow.getShouldClose()) {
		curAngle += 2.0f;
		if (curAngle >= 360) {
			curAngle -= 360;
		}

		return curAngle;
	}
}

bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

// Function to get size to scale object
float getCurSize() {
	while (!mainWindow.getShouldClose()) {
		if (sizeDirection) {
			curSize += 0.001f;
		}

		else if (!sizeDirection) {
			curSize -= 0.001f;
		}

		if (curSize >= maxSize || curSize <= minSize) {
			sizeDirection = !sizeDirection;
		}

		return curSize;
	}
}

// Function to convert from degrees to radians
float rad = 0.0f;
float toRadians(float deg) {
	rad = deg * (M_PI / 180.0f);
	return rad;
}

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

float tOff = 0, cARad = 0;


void calcAverageNormals(const unsigned int *indices, unsigned int indiceCount, GLfloat *vertices, unsigned int verticeCount, unsigned int vLength, unsigned int normalOffset) {
	unsigned int in0 = 0, in1 = 0, in2 = 0, nOffset = 0;
	for (size_t i = 0; i < indiceCount; i += 3) {
		in0 = indices[i] * vLength;
		in1 = indices[i + 1] * vLength;
		in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset;
		in1 += normalOffset;
		in2 += normalOffset;

		vertices[in0] += normal.x;
		vertices[in0 + 1] += normal.y;
		vertices[in0 + 2] += normal.z;

		vertices[in1] += normal.x;
		vertices[in1 + 1] += normal.y;
		vertices[in1 + 2] += normal.z;

		vertices[in2] += normal.x;
		vertices[in2 + 1] += normal.y;
		vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++) {
		nOffset = (i * vLength) + normalOffset;
		glm::vec3 vector(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vector = glm::normalize(vector);
		vertices[nOffset] = vector.x;
		vertices[nOffset + 1] = vector.y;
		vertices[nOffset + 2] = vector.z;
	}
}

void CreateObjects() {
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};


	GLfloat vertices[] = {
	//	x		y	  z				u	  v			nx	  ny	nz
		-1.0f, -1.0f, 0.0f,			0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 1.0f,			0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 0.0f,			1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,			0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,		10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,		0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,			10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);
}

void CreateShader() {
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	DirShadowShader.CreateFromFiles("Shaders/direction_shadow_map.vert", "Shaders/directional_shadow_map.frag");
	// OmniShadowShader.CreateFromFiles("Shaders/Omni_shadow_map.vert", "Shaders/Omni_shadow_map.geom", "Shaders/Omni_shadow_map.frag");
}

void RenderScene() {
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
	//model = glm::rotate(model, cARad, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	brickTexture.UseTexture();
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[0]->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));
	//model = glm::rotate(model, -(cARad), glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dirtTexture.UseTexture();
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[1]->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, -2.5f));
	//model = glm::rotate(model, -(cARad), glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	plainTexture.UseTexture();
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-17.0f, 0.0f, 15.0f));
	//model = glm::rotate(model, -(cARad), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	xWing.RenderModel();

	blackHawkAngle += 0.1f;
	if (blackHawkAngle >= 360.0f) {
		blackHawkAngle = 0.1f;
	}

	model = glm::mat4(1.0f);
	model = glm::rotate(model, toRadians(-blackHawkAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(-8.0f, 2.0f, 0.0f));
	model = glm::rotate(model, toRadians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, toRadians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	blackHawk.RenderModel();
}

void DirShadowMapPass(DirectionalLight* light) {
	DirShadowShader.UseShader();

	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	light->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	glm::mat4 *lTrans = &light->CalculateLightTransform();

	uniformModel = DirShadowShader.GetModelLocation();
	DirShadowShader.SetDirLightTransform(&light->CalculateLightTransform());

	RenderScene();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//void OmniShadowMapPass(PointLight* light) {
//	OmniShadowShader.UseShader();
//
//	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());
//
//	light->GetShadowMap()->Write();
//	glClear(GL_DEPTH_BUFFER_BIT);
//
//	uniformModel = OmniShadowShader.GetModelLocation();
//	uniformOmniLightPos = OmniShadowShader.GetOmniLightPosLocation();
//	uniformFarPlane = OmniShadowShader.GetFarPlaneLocation();
//
//	glm::vec3 lightPos = light->GetPosition();
//
//	glUniform3f(uniformOmniLightPos, lightPos.x, lightPos.y, lightPos.z);
//	glUniform1f(uniformFarPlane, light->GetFarPlane());
//
//	OmniShadowShader.SetLightMatrices(light->CalculateLightTransform());
//
//	RenderScene();
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//}

void RenderPass(glm::mat4 projMat, glm::mat4 viewMat, glm::vec3 cPos) {
	shaderList[0].UseShader();
	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShininess = shaderList[0].GetShininessLocation();

	glViewport(0, 0, 1600, 900);

	// Clear window
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Fully black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projMat));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMat));
	glUniform3f(uniformEyePosition, cPos.x, cPos.y, cPos.z);
	
	glm::mat4 *lightTrans = &mainLight.CalculateLightTransform();

	shaderList[0].SetDirectionalLight(&mainLight);
	shaderList[0].SetPointLights(pointLights, pointLightCount);
	shaderList[0].SetSpotLights(spotLights, spotLightCount);
	shaderList[0].SetDirLightTransform(lightTrans);

	mainLight.GetShadowMap()->Read(GL_TEXTURE1);
	shaderList[0].SetTexture(0);
	shaderList[0].SetDirShadowMap(1);

	glm::vec3 lowerLight = cPos;
	lowerLight.y -= 0.3f;
	//spotLights[0].SetFlashLight(lowerLight, camDir);

	RenderScene();
}

int main() {
	

	mainWindow = MyWindow(1600, 900);
	mainWindow.initialize();

	CreateObjects();
	CreateShader();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 1.0f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();

	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();

	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();

	shinyMaterial = Material(4.0f, 256);
	dullMaterial = Material(0.3f, 4);

	xWing = Model();
	xWing.LoadModel("Models/x-wing.obj");

	blackHawk = Model();
	blackHawk.LoadModel("Models/uh60.obj");

	mainLight = DirectionalLight(1024*5, 1024*5,
								1.0f, 1.0f, 1.0f, 
								0.1f, 0.6f, 
								0.0f, -15.0f, -10.0f);

	

	pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,
								0.0f, 0.1f,
								0.0f, 0.0f, 0.0f,
								0.3f, 0.2f, 0.1f);
	pointLightCount++;

	pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,
								0.0f, 0.1f,
								-4.0f, 2.0f, 0.0f,
								0.3f, 0.1f, 0.1f);
	pointLightCount++;

	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
							0.0f, 2.0f,
							0.0f, 0.0f, 0.0f,
							0.0f, -1.0f, 0.0f,
							1.0f, 0.0f, 0.0f,
							20.0f);
	spotLightCount++;

	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
							0.0f, 1.0f,
							0.0f, -1.5f, 0.0f,
							-100.0f, -1.0f, 0.0f,
							1.0f, 0.0f, 0.0f,
							20.0f);
	spotLightCount++;

	
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	glm::vec3 camPos(0.0f, 0.0f, 0.0f);
	glm::vec3 camDir(0.0f, 0.0f, 0.0f);

	// Loop until window is closed
	while (!mainWindow.getShouldClose()) {
		GLfloat now = glfwGetTime(); // SDL_GetPerformanceTimer();
		deltaTime = now - lastTime; // (now - lastTime)*1000/SDL_GetPerformanceFrequency();
		lastTime = now;

		// Get + Handle user input events
		glfwPollEvents();

		camera.KeyControl(mainWindow.getKeys(), deltaTime);
		camera.MouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		/*tOff = getTriOffset();
		cARad = toRadians(getCurAngle());*/

		camPos = camera.getCameraPostion();
		camDir = camera.getCameraDirection();

		DirShadowMapPass(&mainLight);

		/*for (size_t i = 0; i < pointLightCount; i++) {
			OmniShadowMapPass(&pointLights[i]);
		}

		for (size_t i = 0; i < spotLightCount; i++) {
			OmniShadowMapPass(&spotLights[i]);
		}*/

		RenderPass(projection, camera.CalculateViewMatrix(), camPos);

		glUseProgram(0);

		mainWindow.SwapBuffers();
	}


	return 0;
}

