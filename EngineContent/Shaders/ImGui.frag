#version 310 es

precision mediump float;
precision mediump int;

layout(location=0) in vec2 fragUV;
layout(location=1) in vec4 fragColor;

layout(location=0) out vec4 outputColor;

// NOTE Need to start at location 1 here due to the vertex shader's uniforms
layout(location=1) uniform sampler2D fontTexture;

void main()
{
	outputColor = fragColor * texture(fontTexture, fragUV);
}