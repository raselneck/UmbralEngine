#version 310 es

precision highp float;

layout(location=0) in vec4 fragColor;

layout(location=0) out vec4 outputColor;

void main()
{
	outputColor = vec4(fragColor.rgb, 1.0);
}