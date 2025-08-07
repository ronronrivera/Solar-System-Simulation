#version 330 core
out vec4 FragColor;

uniform sampler2D _texture;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float constant;
uniform float linear;
uniform float quadratic;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);

    // ==AMBIENT==
    float ambientStrength = 0.5f;
    vec3 ambient = ambientStrength * vec3(texture(_texture, TexCoords));
    
    // ==DIFFUSE==
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(texture(_texture, TexCoords));
    
    // ==SPECULAR== (FIXED)
    float specularStrength = 0.005f;
    float shininess = 32.0f;
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * vec3(1.0);
    
    // ==FINAL RESULT==
    vec3 result = (ambient + diffuse + specular)* attenuation;
    FragColor = vec4(result, 1.0);
}
