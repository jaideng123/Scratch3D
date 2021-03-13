#version 400 core

// Incoming from vertex shader
// ALL INPUTS are interpolated from vertex shader
in vec3 VertexColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(VertexColor,1.0);
}