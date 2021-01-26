#version 400 core
// Input Vector3 as aPos at Location 0
// Can also omit layout and use glGetAttribLocation()
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
// will be available in frag shader
out vec3 VertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform uint entityId;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    uint mask = 0xFF;
    float r = (entityId & mask) / 255.0;
    float g = ((entityId >> 8) & mask) / 255.0;
    float b = ((entityId >> 16) & mask) / 255.0;
    VertexColor = vec3(r, g, b);
}