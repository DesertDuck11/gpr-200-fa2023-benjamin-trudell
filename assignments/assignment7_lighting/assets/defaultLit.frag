#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 WorldPosition;
	vec3 WorldNormal;
}fs_in;

struct Light
{
	vec3 position;
	vec3 color;
};

struct Material
{
	float ambientK;
	float diffuseK;
	float specular;
	float shininess;
};

#define MAX_LIGHTS 4
uniform Light _Lights[MAX_LIGHTS];

uniform Material _Material;
uniform sampler2D _Texture;

void main(){
	vec3 normal = normalize(fs_in.WorldNormal);
	//vec3 diffuse = normal * lightDir;
	FragColor = texture(_Texture,fs_in.UV);
	//FragColor = vec4(diffuse,fs_in.UV);
}