#version 450 
out vec4 FragColor; 
in vec2 UV;

uniform sampler2D _SkyTexture; 
uniform sampler2D _SunTexture;
uniform sampler2D _DistortTexture;

uniform float _DistStrength;
uniform float _Tiling;
uniform float iTime;

void main(){
    float normalizedTime = sin(iTime) * 0.1;

    float distort = texture(_DistortTexture,UV).r * normalizedTime;

    vec2 uv = UV;
    uv += distort * _DistStrength;

    vec4 texA = texture(_SkyTexture, (uv * 2.0) * _Tiling);
    vec4 texB = texture(_SunTexture, (uv) * _Tiling);

    vec3 color = mix(texA.rgb, texB.rgb, texB.a);

    FragColor = vec4(color, 1.0);
}
