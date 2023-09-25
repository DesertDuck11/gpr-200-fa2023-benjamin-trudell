#version 450
out vec4 FragColor;
in vec2 UV;
//uniform float iTime;
void main(){
	vec3 color = mix(vec3(0.1,0.0,0.0),vec3(0.5,0.0,1.0),UV.y);
    float hills = 0.5 - step(sin(UV.x*12.56) * 0.15 + 0.2,UV.y);
    color = mix(color,vec3(0.2),hills);
    FragColor = vec4(color,1.0);
}

/*float circleSDF(vec2 p, float r){
    return length(p)-r;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord/iResolution.xy;
    
    vec3 dayColor = mix(vec3(0.5,0.0,0.5),vec3(0.5,0.0,0.5),uv.y);
    vec3 nightColor = mix(vec3(0.0,0.0,0.5),vec3(0.0,0.0,0.5),uv.y);
    vec4 sunColor = vec4(mix(vec3(0.5,0.0,0.1),vec3(0.5,0.0,1.0),uv.y),1.0);
    
    vec3 backgroundColor = mix(dayColor, nightColor, uv.y + cos(iTime));
    
    vec3 mountainColor = mix(vec3(0.0,0.05,0.15), vec3(0.0, 0.0, 0.0), (uv.y) + cos(iTime));
    
    vec3 outColor = backgroundColor;
   
    uv = uv * 2.0 - 1.0;
    uv.x *= iResolution.x / iResolution.y;

    vec2 sunPos = vec2(0.0, -0.65 - cos(iTime));
    
    float sun = circleSDF(uv - sunPos, 0.3);
    sun = 1.0 - smoothstep(-0.05, 0.05, sun);
    
    outColor = mix(outColor, sunColor.rgb, sun*sunColor.a);
    
    float mountains = 1.0 - step(sin(uv.x*5.75)*0.5 - 0.25,uv.y);
    
    outColor = mix(outColor, mountainColor, mountains * 1.0);
    
    fragColor = vec4(outColor, 1.0);
}*/
