#version 450
out vec4 FragColor;
in vec2 UV;
uniform float iTime;
uniform vec2 iResolution;
uniform vec3 SkyColor[2];
uniform vec3 MountainColor[4];
uniform vec3 SunColor[1];
uniform float SunRadius;
uniform float SunSpeed;

float circleSDF(vec2 p, float r){
    return length(p)-r;
}

void main(){
    vec2 uv = UV;

    // Day/Night Color (Background)
	vec3 dayColor = mix(SkyColor[0], SkyColor[0], uv.y);
    vec3 nightColor = mix(SkyColor[1], SkyColor[1], uv.y);

    // Sun Color
    vec4 sunColor = vec4(SunColor[0], 1.25);
    
    // Background Color
    vec3 backgroundColor = mix(dayColor, nightColor, uv.y + cos(iTime * SunSpeed));
    
    // Mountain Colors (Foreground)
    vec3 mountainColor1 = mix(MountainColor[0], MountainColor[1], (uv.y) + cos(iTime * SunSpeed));
    vec3 mountainColor2 = mix(MountainColor[2], MountainColor[3], (uv.y) + cos(iTime * SunSpeed));
    
    vec3 outColor = backgroundColor;
   
    uv = uv * 2.0 - 1.0;
    uv.x *= (iResolution.x / iResolution.y) * 2.25;

    // Sun Position + Movement
    vec2 sunPos = vec2(0.0, -0.65 - cos(iTime * SunSpeed));

    float sun = circleSDF(uv - sunPos, SunRadius);
    sun = 1.0 - smoothstep(-0.05, 0.05, sun);
    
    // Add Sun to Draw Call
    outColor = mix(outColor, sunColor.rgb, sun*sunColor.a);

    // Mountain Functions
    float mountains1 = 1.0 - step(0.0 + (sin(uv.x * 5.0) * cos(uv.x/0.4) - (uv.x * uv.x)) / 2.0,uv.y);
    float mountains2 = 1.0 - step(-0.65 + sin(uv.x * 1.5) * cos(uv.x/0.35) / 3.5,uv.y);
    
    // Add Mountains (Foreground) & Background to Draw Call
    outColor = mix(outColor, mountainColor1, mountains1 * 1.0);
    outColor = mix(outColor, mountainColor2, mountains2 * 1.0); 
    
    // Draw Call
    FragColor = vec4(outColor, 1.0);
}
