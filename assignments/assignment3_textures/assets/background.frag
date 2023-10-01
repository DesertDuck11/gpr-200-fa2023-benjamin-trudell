#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _BrickTexture;

void main(){
	FragColor = texture(_BrickTexture,UV);
}
