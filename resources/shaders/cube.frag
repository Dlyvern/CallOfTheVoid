#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

struct Light
{
    vec3 position;
    vec3 color;
    float strength;
    float radius;
};

uniform sampler2D tex0;

uniform Light light;

uniform vec3 viewPos;

void main()
{
     vec3 color = texture(tex0, fs_in.TexCoords).rgb;
    // ambient
    vec3 ambient = 0.05 * color;
    // diffuse
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(light.strength) * spec; // assuming bright white light color
    FragColor = vec4(ambient + diffuse + specular, 1.0);

    // vec3 color = texture(tex0, fs_in.TexCoords).rgb;

    // // ambient
    // vec3 ambient = 0.05 * color;

    // // diffuse
    // vec3 lightDir = normalize(light.position - fs_in.FragPos);
    // float distance = length(light.position - fs_in.FragPos);
    // float attenuation = 1.0 / (distance * distance);
    // vec3 normal = normalize(fs_in.Normal);
    // float diff = max(dot(lightDir, normal), 0.0);
    // vec3 diffuse = diff * light.color * light.strength * attenuation;

    // // specular
    // vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    // vec3 reflectDir = reflect(-lightDir, normal);
    // float spec = 0.0;
    // vec3 halfwayDir = normalize(lightDir + viewDir);  
    // spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	// vec3 specular = spec * light.color * light.strength * attenuation;; // assuming bright white light color
   
    // FragColor = vec4(ambient + diffuse + specular, 1.0);
}