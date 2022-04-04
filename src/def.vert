#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;

out vec3 fragPos;
out vec3 normal;

void main(){
    gl_Position = vec4(aPos, 1);
    fragPos = vec3(model*vec4(aPos,1));
    normal = normalize(aNormal);
}