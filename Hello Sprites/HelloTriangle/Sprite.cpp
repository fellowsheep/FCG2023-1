#include "Sprite.h"

Sprite::~Sprite()
{
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
}

void Sprite::initialize(GLuint texID, glm::vec2 sprDimensions, Shader* shader, int nAnimations, int nFrames, glm::vec3 position, glm::vec3 scale, float angle, glm::vec3 axis)
{
	this->texID = texID;
	this->sprDimensions = sprDimensions;
	this->shader = shader;
	this->position = position;
	this->scale = glm::vec3(sprDimensions.x / (float) nFrames, sprDimensions.y / (float)nAnimations, 1.0);
	this->angle = angle;
	this->nAnimations = nAnimations;
	this->nFrames = nFrames;
	this->axis = axis;

	dx = 1.0 / (float)nFrames;
	dy = 1.0 / (float)nAnimations;

	iFrame = 0;
	iAnimation = 0;
	vel = 5.0;

	GLfloat vertices[] = {
		//Primeiro triangulo
		//x   y     z    r    g    b    s	t
		-0.5,  0.5, 0.0, 1.0, 0.0, 0.0, 0.0, dy,  //v0
		-0.5, -0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, //v1
		 0.5,  0.5, 0.0, 0.0, 0.0, 1.0, dx,  dy,  //v2
		//Segundo triângulo
		-0.5, -0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, //v1
		 0.5, -0.5, 0.0, 0.0, 1.0, 0.0, dx,  0.0, //v3
		 0.5,  0.5, 0.0, 0.0, 0.0, 1.0, dx,  dy   //v2

	};

	GLuint VBO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
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

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

}

void Sprite::update()
{
	//Matriz de modelo: transformações no objeto
	glm::mat4 model = glm::mat4(1); //matriz identidade

	model = glm::translate(model, position);
	model = glm::rotate(model, angle, axis);
	model = glm::scale(model, scale);

	//Enviando a matriz de modelo para o shader
	shader->setMat4("model", glm::value_ptr(model));

	//Enviando para o shader o deslocamento das coords de textura
	//de acordo com os indices de animação e frame
	iFrame = (iFrame + 1) % nFrames;
	shader->setVec2("offsets", iFrame * dx, iAnimation * dy);

}

void Sprite::draw()
{

	glBindVertexArray(VAO); //Conectando ao buffer de geometria desejado
	glBindTexture(GL_TEXTURE_2D, texID); //Conectando ao buffer de textura desejado

	// Chamada de desenho - drawcall
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0); //Desconectando o buffer de geometria
	glBindTexture(GL_TEXTURE_2D, 0);

}

void Sprite::moveRight()
{
	position.x += vel;
}

void Sprite::moveLeft()
{
	position.x -= vel;
}

AABB Sprite::getAABB()
{
	AABB pontos;

	pontos.pmin.x = position.x - sprDimensions.x / 2;
	pontos.pmin.y = position.y - sprDimensions.y / 2;

	pontos.pmax.x = position.x + sprDimensions.x / 2;
	pontos.pmax.y = position.y + sprDimensions.y / 2;
	
	return pontos;
}
