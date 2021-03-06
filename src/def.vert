#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform vec3 orginPos;
uniform mat4 camMatrix;

out vec3 fragPos;
out vec3 normal;

void main(){
    gl_Position = camMatrix * vec4(orginPos + aPos, 1);
    fragPos = vec3(orginPos + aPos);
    normal = normalize(aNormal);
}