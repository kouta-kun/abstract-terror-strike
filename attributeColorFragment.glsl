#version 330 core

uniform vec3 fragmentColor;

in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

out vec3 color;

void main() {
    vec3 n = normalize(Normal_cameraspace);
    vec3 l = normalize(LightDirection_cameraspace);

    float cosTheta = clamp(dot(n, l), 0, 1);

    color = fragmentColor * 25.0f * cosTheta;
}
