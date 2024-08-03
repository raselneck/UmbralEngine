#version 310 es

precision highp float;

layout(location=0) in vec2 FragmentUV;

layout(location=0) out vec4 FinalColor;

layout(location=0) uniform sampler2D QuadTexture;

void main()
{
	vec4 textureColor = texture(QuadTexture, FragmentUV);
	FinalColor = vec4(textureColor.rgb, 1);
}