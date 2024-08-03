#version 310 es

precision highp float;

layout(location=0) in vec3 vertNormal;
layout(location=1) in vec2 vertUV;

layout(location=0) out vec4 fragColor;

layout(location=3) uniform sampler2D diffuseTexture;
layout(location=4) uniform vec3 lightDirection;

void main()
{
	float NdotL = dot(vertNormal, lightDirection);
	float intensity = clamp(NdotL, 0.1f, 1.0f);

	vec2 uv = vec2(vertUV.x, 1.0f - vertUV.y);
	vec4 diffuseColor = texture(diffuseTexture, uv);

	fragColor = vec4(diffuseColor.rgb * intensity, 1.0f);
}