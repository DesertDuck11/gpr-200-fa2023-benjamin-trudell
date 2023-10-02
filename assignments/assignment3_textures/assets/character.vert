#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;
out vec2 UV;

uniform float iTime;
uniform float _moveSpeed;

void main(){
	UV = vUV;
	vec3 Pos = vPos;

	float movementX = sin(iTime * _moveSpeed);
    float movementY = cos((iTime* _moveSpeed) * 0.5) ; 

    Pos += vec3(movementX * 0.2, movementY * 0.2, 0.0);

	gl_Position = vec4(Pos, 2.0);
}