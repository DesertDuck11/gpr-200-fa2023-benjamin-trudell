#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

out vec3 Normal;

uniform mat4 _Model1;
uniform mat4 _Model2;
uniform mat4 _Model3;
uniform mat4 _Model4;

void main(){
	Normal = vNormal;
	gl_Position = _Model1 * vec4(vPos,1.0);
	gl_Position.z*=-1.0;
}