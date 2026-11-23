#version 330 
// this shader needs a better name...

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColour;
layout (location = 2) in vec2 aUv;
//layout (location = 4) in uint aTextureid;

out vec3 vPosition;
out vec4 vColour;
out vec2 vUV;
//out uint vTextureid;

uniform mat4 u_ViewMatrix;

void main()
{
    gl_Position = u_ViewMatrix * vec4(aPosition, 1.0);
    //gl_Position = vec4(
    //    aPosition.xy / 100.0 - 1.0,
    //    0.0,
    //    1.0
    //);
    vPosition = aPosition;
    vColour = aColour;
    //vColour = vec4(0.0, 1.0, 1.0, 1.0);
    vUV = aUv;
    //vTextureid = aTextureid;
}