#version 460 core


 out vec4 FragColor;



in vec3 fragPos;
in vec3 normal;


const float specStrength = 0.4;
const int specularPower = 16;

void main(){
    FragColor = vec4(0.1, 0.1, 0.1, 1);
}