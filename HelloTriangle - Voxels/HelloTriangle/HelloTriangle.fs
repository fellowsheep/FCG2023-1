//Código fonte do Fragment Shader (em GLSL)
#version 460

in vec4 finalColor;
in vec2 texCoord;
out vec4 color;

uniform sampler2D texBuff;

void main()
{
	color = finalColor;//texture(texBuff,texCoord);//
}