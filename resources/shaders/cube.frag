#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

//#define MAX_LIGHTS = 100

struct Light
{
    vec3 position;
    vec3 color;
    float strength;
    float radius;

    vec3 direction;
    float cutoff;
    float outerCutoff;
};

uniform sampler2D tex0;

uniform Light lights[100];
uniform vec3 viewPos;

uniform Light light;

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
}

//vec3 color = texture(tex0, fs_in.TexCoords).rgb;
//vec3 normal = normalize(fs_in.Normal);
//// ambient
//vec3 ambient = 0.1 * color;
//vec3 result = ambient; // start with ambient light
//
//for (int i = 0; i < MAX_LIGHTS; ++i)
//{
//if (length(lights[i].position) > 0.0f) // if light is valid
//{
//// Calculate lighting for each light (same as before)
//vec3 lightDir = normalize(lights[i].position - fs_in.FragPos);
//float diff = max(dot(normal, lightDir), 0.0f);
//vec3 diffuse = diff * lights[i].color;
//
//// Specular lighting
//vec3 viewDir = normalize(viewPos - fs_in.FragPos);
//vec3 reflectDir = reflect(-lightDir, normal);
//float spec = pow(max(dot(normal, reflectDir), 0.0f), 32.0);
//vec3 specular = lights[i].strength * spec * vec3(1.0f);
//result += diffuse + specular;
//}
//}
//
//FragColor = vec4(result, 1.0);
