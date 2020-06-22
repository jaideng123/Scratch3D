#version 400 core
// Incoming from vertex shader
// ALL INPUTS are interpolated from vertex shader
in vec3 vertexColor;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

out vec4 FragColor;

void main()
{
    FragColor = texture(texture_diffuse1, TexCoords);
} 