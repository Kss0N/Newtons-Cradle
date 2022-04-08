#version 460 core


 out vec4 FragColor;



in vec3 fragPos;
in vec3 normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;
uniform vec3 lightColor;


const float specStrength = 0.4;
const int specularPower = 16;

void main(){
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflDir = reflect(-lightDir, normal);

    float ambientFactor = 0.8;
    float diffuseFactor = max(dot(normal, lightDir), 0);
    float specularFactor = specStrength * pow(max(dot(viewDir, reflDir), 0.0), specularPower);

    vec3 result = (ambientFactor + diffuseFactor +  specularFactor) * lightColor;
    FragColor = vec4(result * objectColor, 1);
}