#version 400 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D emission;
    float shininess;
};

// Incoming from vertex shader
// ALL INPUTS are interpolated from vertex shader
in vec3 vertexColor;
in vec2 TexCoords;

uniform Material material;
uniform bool highlighted;

out vec4 FragColor;

void main()
{
    vec4 color = texture(material.texture_diffuse1, TexCoords);
    if(highlighted){
        color += vec4(0,0.5f,0,0);
    }
    FragColor = color;
} 