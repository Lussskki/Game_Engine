#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

out vec3 v_Color;
out vec3 v_WorldPosition;

void main()
{
    vec4 worldPosition = u_Model * vec4(a_Position, 1.0);
    v_Color = a_Color;
    v_WorldPosition = worldPosition.xyz;
    gl_Position = u_ViewProjection * worldPosition;
}
