#version 310 es

precision highp float;

layout(location=0) in vec3 vertNormal;

layout(location=0) out vec4 fragColor;

layout(location=3) uniform vec4 diffuseColor;
layout(location=4) uniform vec3 lightDirection;

void main()
{
	float NdotL = dot(vertNormal, lightDirection);
	float intensity = clamp(NdotL, 0.1f, 1.0f);

	fragColor = vec4(diffuseColor.rgb * intensity, 1.0f);
}