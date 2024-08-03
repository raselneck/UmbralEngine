#version 450 core

layout(location=0) in vec2 Frag_UV;
layout(location=1) in vec4 Frag_Color;

layout(location=0) out vec4 Out_Color;

layout(location=1) uniform sampler2D FontTexture;

void main()
{
	Out_Color = Frag_Color * texture(FontTexture, Frag_UV.st);
}