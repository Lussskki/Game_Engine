#version 330 core

in vec3 v_Color;
in vec3 v_WorldPosition;
out vec4 FragColor;

uniform int u_UseSolidColor;
uniform vec3 u_SolidColor;
uniform vec3 u_MaterialColor;
uniform vec3 u_LightDirection;
uniform float u_AmbientStrength;
uniform float u_LightIntensity;
uniform float u_Alpha;

void main()
{
    vec3 normal = normalize(cross(dFdx(v_WorldPosition), dFdy(v_WorldPosition)));
    vec3 lightDirection = normalize(-u_LightDirection);
    float diffuse = max(dot(normal, lightDirection), 0.0) * u_LightIntensity;
    vec3 baseColor = v_Color * u_MaterialColor;
    vec3 litColor = baseColor * (u_AmbientStrength + diffuse);
    vec3 color = u_UseSolidColor == 1 ? u_SolidColor : litColor;
    FragColor = vec4(color, u_Alpha);
}
