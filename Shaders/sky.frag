#version 330 core

in vec2 v_UV;
out vec4 FragColor;

uniform float u_CameraYaw;
uniform float u_CameraPitch;
uniform float u_AspectRatio;
uniform vec3 u_LightDirection;
uniform float u_AmbientStrength;
uniform float u_LightIntensity;

vec3 NormalizeSafe(vec3 value)
{
    float lengthValue = length(value);
    if (lengthValue <= 0.0001)
    {
        return vec3(0.0, 1.0, 0.0);
    }

    return value / lengthValue;
}

void main()
{
    vec3 cameraForward = NormalizeSafe(vec3(cos(u_CameraYaw) * cos(u_CameraPitch), sin(u_CameraPitch), sin(u_CameraYaw) * cos(u_CameraPitch)));
    vec3 cameraRight = NormalizeSafe(cross(cameraForward, vec3(0.0, 1.0, 0.0)));
    vec3 cameraUp = cross(cameraRight, cameraForward);

    vec2 ndc = v_UV * 2.0 - 1.0;
    float tanHalfFov = 0.41421356;
    vec3 viewRay = NormalizeSafe(cameraForward + cameraRight * ndc.x * u_AspectRatio * tanHalfFov + cameraUp * ndc.y * tanHalfFov);

    float skyY = clamp(viewRay.y * 0.5 + 0.5, 0.0, 1.0);
    float ambient = clamp(u_AmbientStrength, 0.0, 1.0);
    float intensity = clamp(u_LightIntensity, 0.0, 3.0);

    vec3 horizon = vec3(0.72, 0.83, 0.92);
    vec3 middleSky = vec3(0.48, 0.66, 0.86);
    vec3 skyTop = vec3(0.22, 0.44, 0.78);
    vec3 duskTint = vec3(0.95, 0.55, 0.32);
    vec3 nightTint = vec3(0.03, 0.06, 0.12);

    vec3 color = mix(horizon, middleSky, smoothstep(0.05, 0.55, skyY));
    color = mix(color, skyTop, smoothstep(0.55, 1.0, skyY));

    vec3 sunDirection = NormalizeSafe(-u_LightDirection);
    float sunHeight = clamp(sunDirection.y * 0.5 + 0.5, 0.0, 1.0);
    float sunsetAmount = smoothstep(0.05, 0.45, 1.0 - sunHeight) * intensity;
    color = mix(color, color * duskTint, clamp(sunsetAmount * 0.45, 0.0, 0.7));

    float exposure = clamp(ambient * 0.60 + intensity * 0.30, 0.08, 1.35);
    color *= exposure;
    color = mix(nightTint, color, clamp(ambient + intensity * 0.45, 0.0, 1.0));

    float sunFacing = dot(viewRay, sunDirection);
    float sunDisc = smoothstep(0.9880, 0.9940, sunFacing);
    float sunGlow = smoothstep(0.8600, 0.9880, sunFacing);
    float sunPower = clamp(intensity, 0.0, 3.0);

    vec3 glowColor = mix(vec3(1.0, 0.55, 0.18), vec3(1.0, 0.84, 0.30), sunHeight);
    vec3 discColor = vec3(1.0, 0.86, 0.12);
    color += glowColor * sunGlow * sunPower * 0.85;
    color = mix(color, discColor, clamp(sunDisc * sunPower, 0.0, 1.0));

    FragColor = vec4(color, 1.0);
}
