//Código fonte do Fragment Shader (em GLSL)
#version 460

in vec4 finalColor;
in vec2 texCoord;
out vec4 color;

uniform sampler2D texBuff;

uniform vec4 inputColor;

void main()
{
	color = inputColor; //* finalColor;//texture(texBuff,texCoord);//
}