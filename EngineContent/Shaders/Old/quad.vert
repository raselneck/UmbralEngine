#version 310 es

precision highp float;

layout(location=0) in vec2 VertexPosition;
layout(location=1) in vec2 VertexUV;

layout(location=0) out vec2 FragmentUV;

layout(location=1) uniform mat4 ProjectionMatrix;

void main()
{
	FragmentUV = VertexUV;
	gl_Position = ProjectionMatrix * vec4(VertexPosition, 0, 1);
}