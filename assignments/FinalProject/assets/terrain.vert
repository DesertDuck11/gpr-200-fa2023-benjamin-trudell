#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

out Surface{
	vec2 UV;
	vec3 WorldPosition;
	vec3 WorldNormal;
}vs_out;

out float heightY;

uniform mat4 _Model;
uniform mat4 _ViewProjection;

void main(){
	vs_out.UV = vUV;
	vec4 vertPos4 = _Model * vec4(vPos, 1.0);
	vs_out.WorldPosition = vec3(vertPos4) / vertPos4.w;
	heightY = vertPos4.y;
	vs_out.WorldNormal = vec3(_Model * vec4(vNormal, 0.0));
	gl_Position = _ViewProjection * _Model * vec4(vPos,1.0);
}