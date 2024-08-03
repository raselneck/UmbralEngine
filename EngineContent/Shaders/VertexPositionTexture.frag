#version 310 es

precision highp float;

layout(location=0) in vec2 fragUV;

layout(location=0) out vec4 outputColor;

// NOTE Need to start at location 3 here due to the vertex shader's uniforms
layout(location=3) uniform sampler2D diffuseTexture;

void main()
{
	vec4 diffuseColor = texture(diffuseTexture, fragUV);
	outputColor = vec4(diffuseColor.rgb, 1.0);
}