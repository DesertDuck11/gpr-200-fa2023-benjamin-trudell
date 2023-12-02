#version 450
out vec4 FragColor;
in vec2 UV;

uniform vec3 dayColor;
uniform vec3 nightColor;
uniform vec3 sunColor;

float circleSDF(vec2 p, float r){
    return length(p)-r;
}

void main(){
	vec3 col = mix(nightColor, dayColor, UV.y);
	vec2 sunPos = vec2(1.5, 0.5);
	float d1 = circleSDF(vec2(UV.x * 2, UV.y) - sunPos, 0.03);
	float d2 = circleSDF(vec2(UV.x * 2, UV.y) - vec2(sunPos.x - 1, sunPos.y), 0.03);

	col = mix(sunColor, col, smoothstep(-0.05, 0.05, d1));
	col = mix(sunColor, col, smoothstep(-0.05, 0.05, d2));
	FragColor = vec4(col, 1.0);
}
