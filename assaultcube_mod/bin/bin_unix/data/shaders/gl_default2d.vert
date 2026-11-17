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
    gl_Position = u_ViewMatrix * vec4(aPosition, 1.0f);
    vPosition = aPosition;
    vColour = aColour;
    vUV = aUv;
    //vTextureid = aTextureid;
}