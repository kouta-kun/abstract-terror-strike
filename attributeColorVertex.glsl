#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 LightPosition_worldspace;

out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;


void main() {
    mat4 MVP = Projection*View*Model;

    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0f);
    Position_worldspace = (Model * vec4(vertexPosition_modelspace, 1)).xyz;

    vec3 vertexPosition_cameraspace = (View * Model * vec4(vertexPosition_modelspace, 1)).xyz;
    EyeDirection_cameraspace = vec3(0, 0, 0) - vertexPosition_cameraspace;

    vec3 LightPosition_cameraspace = (View * vec4(LightPosition_worldspace, 1)).xyz;
    LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

    Normal_cameraspace = (View * Model * vec4(vertexNormal_modelspace, 0)).xyz;
}