#version 450
out vec4 FragColor;
in vec2 UV;

uniform vec3 skyColor[4];
uniform vec3 sunColor[2];
uniform int dayNight;

float circleSDF(vec2 p, float r){
    return length(p)-r;
}

void main(){
	if(dayNight == 0) {
		vec3 col = mix(skyColor[0], skyColor[1], UV.y);
		vec2 sunPos = vec2(3, 0.5);
		float d1 = circleSDF(vec2(UV.x * 2, UV.y) - sunPos, 0.03);
		float d2 = circleSDF(vec2(UV.x * 2, UV.y) - vec2(sunPos.x - 2, sunPos.y), 0.03);

		col = mix(sunColor[0], col, smoothstep(-0.05, 0.05, d1));
		col = mix(sunColor[0], col, smoothstep(-0.05, 0.05, d2));
		FragColor = vec4(col, 1.0);
	}
	else {
		vec3 col = mix(skyColor[2], skyColor[3], UV.y);
		vec2 sunPos = vec2(3, 0.5);
		float d1 = circleSDF(vec2(UV.x * 2, UV.y) - sunPos, 0.03);
		float d2 = circleSDF(vec2(UV.x * 2, UV.y) - vec2(sunPos.x - 2, sunPos.y), 0.03);

		col = mix(sunColor[1], col, smoothstep(-0.05, 0.05, d1));
		col = mix(sunColor[1], col, smoothstep(-0.05, 0.05, d2));
		FragColor = vec4(col, 1.0);
	}
}
