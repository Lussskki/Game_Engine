#version 330 core

in vec3 v_Color;
out vec4 FragColor;

uniform int u_UseSolidColor;
uniform vec3 u_SolidColor;

void main()
{
    vec3 color = u_UseSolidColor == 1 ? u_SolidColor : v_Color;
    FragColor = vec4(color, 1.0);
}
