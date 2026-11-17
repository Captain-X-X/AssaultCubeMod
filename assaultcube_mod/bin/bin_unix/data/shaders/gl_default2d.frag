#version 330 core
// this shader needs a better name...

out vec4 FragColor;
in vec3 vPosition;
in vec4 vColour;
in vec2 vUV;
//in uint vTextureid;

uniform sampler2D u_Texture0;

void main()
{
	FragColor = vColour;
    FragColor = texture(u_Texture0, vUV);
}