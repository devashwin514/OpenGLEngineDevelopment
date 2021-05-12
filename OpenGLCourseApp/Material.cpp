#include "Material.h"



Material::Material()
{
	specularIntensity = shininess = 0.0f;
}

Material::Material(GLfloat sIntensity, GLfloat shineValue)
{
	specularIntensity = sIntensity;
	shininess = shineValue;

}

void Material::UseMaterial(GLuint specularIntensityLocation, GLuint shineLocation) {
	glUniform1f(specularIntensityLocation, specularIntensity);
	glUniform1f(shineLocation, shininess);
}


Material::~Material()
{
}
