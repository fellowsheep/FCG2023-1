//Código fonte do Fragment Shader (em GLSL)
#version 460

in vec4 finalColor;
out vec4 color;

void main()
{
	color = finalColor;
}