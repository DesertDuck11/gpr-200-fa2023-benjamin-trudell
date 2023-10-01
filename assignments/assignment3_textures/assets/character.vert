#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;
out vec2 UV;

uniform float iTime;

void main(){
	UV = vUV;

	float movementX = sin(iTime);
    float movementY = cos(iTime * 0.5); 

    UV += vec2(movementX * 0.1, movementY * 0.1);

	gl_Position = vec4(vPos, 2.0);
}