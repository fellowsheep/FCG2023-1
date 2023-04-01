// Código fonte do Vertex Shader (em GLSL)
#version 460

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertex_color;

out vec4 finalColor;

uniform mat4 projection;

void main()
{
	//...pode ter mais linhas de código aqui!
	gl_Position = projection * vec4(position, 1.0);
	//finalColor = vec4(vertex_color.r, vertex_color.g, vertex_color.b, 1.0);
	finalColor = vec4(vertex_color, 1.0);
}