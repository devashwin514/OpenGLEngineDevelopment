#version 330																
																			
																			
in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 DirLightSpacePos;																
																			
out vec4 color;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

struct Light {
	vec3 colour;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight {
	Light base;
	vec3 direction;
};

struct PointLight {
	Light base;
	vec3 position;
	float constant;
	float linear;
	float exponent;
};

struct SpotLight {
	PointLight base;
	vec3 direction;
	float edge;
};

struct Material {
	float specularIntensity;
	float shininess;
};

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform sampler2D theTexture;
uniform sampler2D dirShadowMap;
uniform Material matrl;
uniform vec3 eyePosition;

float CalcDirectionalShadowFactor(DirectionalLight light) {
	vec3 projCoords = DirLightSpacePos.xyz / DirLightSpacePos.w;
	projCoords = (projCoords * 0.5f) + 0.5;

	float currentDepth = projCoords.z;

	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(light.direction);

	float bias = max(0.05 * (1 - dot(normal, lightDir)), 0.005);

	float shadow = 0.0f;

	vec2 texelSize = 1.0f / textureSize(dirShadowMap, 0);

	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			float pcfDepth = texture(dirShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += (currentDepth - bias) > pcfDepth ? 1.0f : 0.0f;
		}
	}

	shadow /= 9.0f;

	if (projCoords.z > 1.0f) {
		shadow = 0.0f;
	}

	return shadow;
}

vec4 CalcLightByDirection(Light light, vec3 direction, float shadowFactor) {
	vec4 ambientColor = vec4(light.colour, 1.0f) * light.ambientIntensity;
	float diffuseFactor = max(dot(normalize(Normal), normalize(direction)), 0.0f);
	vec4 diffuseColor = vec4(light.colour * light.diffuseIntensity * diffuseFactor, 1.0f);
	vec4 specularColor = vec4(0, 0, 0, 0);
	if (diffuseFactor > 0.0f) {
		vec3 fragToEye = normalize(eyePosition - FragPos);
		vec3 reflectedVertex = normalize(reflect(direction, normalize(Normal)));
		float specularFactor = dot(fragToEye, reflectedVertex);
		if (specularFactor > 0.0f) {
			specularFactor = pow(specularFactor, matrl.shininess);
			specularColor = vec4(light.colour * matrl.specularIntensity * specularFactor, 1.0f);
		}
	}
	return (ambientColor + (1.0f - shadowFactor) * (diffuseColor + specularColor));
}

vec4 CalcDirectionalLight() {
	float sf = CalcDirectionalShadowFactor(dirLight);
	return CalcLightByDirection(dirLight.base, dirLight.direction, sf);
}

vec4 CalcPointLight(PointLight pLight) {
	vec3 dir = FragPos - pLight.position;
	float distance = length(dir);
	dir = normalize(dir);
	vec4 col = CalcLightByDirection(pLight.base, dir, 0.0f);
	float attenuation = (pLight.exponent * distance * distance) + 
						(pLight.linear * distance) + (pLight.constant);
	return (col / attenuation);
}

vec4 CalcSpotLight(SpotLight sLight) {
	vec3 rayDirection = normalize(FragPos - sLight.base.position);
	float slFactor = dot(rayDirection, sLight.direction);

	if (slFactor > sLight.edge) {
		vec4 c = CalcPointLight(sLight.base);
		return c * (1.0f - (1.0f - slFactor) * (1.0f / (1.0f - sLight.edge)));
	}

	else {
		return vec4(0, 0, 0, 0);
	}
}

vec4 CalcPointLights() {
	vec4 totalColor = vec4(0, 0, 0, 0);
	for (int i = 0; i < pointLightCount; i++) {
		totalColor += CalcPointLight(pointLights[i]);
	}
	return totalColor;
}

vec4 CalcSpotLights() {
	vec4 totalColor = vec4(0, 0, 0, 0);
	for (int i = 0; i < pointLightCount; i++) {
		totalColor += CalcSpotLight(spotLights[i]);
	}
	return totalColor;
}
																			
void main() {
	vec4 finalColor = CalcDirectionalLight();
	finalColor += CalcPointLights();
	finalColor += CalcSpotLights();											
	color = texture(theTexture, TexCoord) * finalColor;															
}