#version 400 core
struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D emission;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

uniform Material material;
uniform DirectionalLight dirLight;
uniform vec3 viewPos;
uniform bool highlighted;
uniform sampler2D texture_diffuse1;

out vec4 FragColor;

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);

void main()
{
    // obtain normal from normal map in range [0,1]
    vec3 normal = normalize(texture(material.texture_normal1, TexCoords).rgb);
    normal = normalize(normal * 2.0 - 1.0);
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);

    vec3 result = CalcDirLight(dirLight, normal, viewDir);

    if(highlighted){
        result += vec3(0,0.5f,0);
    }

    FragColor = vec4(result,1);
}

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(lightDir, normal), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    // Calculate Specular with Blinn-Phong
    float spec = pow(max(dot(normal, halfwayDir), 0.0),material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
    return (ambient + diffuse + specular);
}