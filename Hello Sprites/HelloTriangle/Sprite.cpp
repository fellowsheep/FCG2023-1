#include "Sprite.h"

Sprite::~Sprite()
{
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
}

void Sprite::initialize(GLuint texID, glm::vec2 sprDimensions, Shader* shader, glm::vec3 position, glm::vec3 scale, float angle, int nAnimations, int nFrames, glm::vec3 axis)
{
	this->texID = texID;
	this->sprDimensions = sprDimensions;
	this->shader = shader;
	this->position = position;
	this->scale = glm::vec3(sprDimensions,1.0);
	this->angle = angle;
	this->nAnimations = nAnimations;
	this->nFrames = nFrames;
	this->axis = axis;

	GLfloat vertices[] = {
		//Primeiro triangulo
		//x   y     z    r    g    b    s	t
		-0.5,  0.5, 0.0, 1.0, 0.0, 0.0, 0.0, 0.1,  //v0
		-0.5, -0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0,  //v1
		 0.5,  0.5, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0   //v2
		//Segundo tri�ngulo
		-0.5, -0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0,  //v1
		 0.5, -0.5, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0,  //v3
		 0.5,  0.5, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0   //v2

	};

	GLuint VBO;

	//Gera��o do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conex�o (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Gera��o do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v�rtices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localiza��o no shader * (a localiza��o dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se est� normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo posicao
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo coordenada de textura 
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

}

void Sprite::update()
{
	//Matriz de modelo: transforma��es no objeto
	glm::mat4 model = glm::mat4(1); //matriz identidade

	model = glm::translate(model, position);
	model = glm::rotate(model, angle, axis);
	model = glm::scale(model, scale);

	//Enviando a matriz de modelo para o shader
	//GLint modelLoc = glGetUniformLocation(shader.ID, "model");
	//glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

}

void Sprite::draw()
{
}
