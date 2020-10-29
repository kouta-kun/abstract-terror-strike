#version 330 core

uniform vec3 ModelColor;

out vec3 color;

void main() {
    color = ModelColor;
}