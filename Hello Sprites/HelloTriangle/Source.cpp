/* Hello Triangle - c�digo adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle 
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gr�fico - Unisinos
 * Vers�o inicial: 7/4/2017
 * �ltima atualiza��o em 27/02/2023
 *
 */

#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Shader.h"
#include "stb_image.h"
//#include "Sprite.h"
#include "Meteor.h"
#include "Timer.h"


// Prot�tipo da fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double mouse_x, double mouse_y);

// Prot�tipos das fun��es
int setupGeometry();
int setupTexture(string texName, int& width, int& height);
bool testCollision(Sprite &a, Sprite &b);

// Dimens�es da janela (pode ser alterado em tempo de execu��o)
const GLuint WIDTH = 800, HEIGHT = 600;

//Sprites globais
Sprite dino;

// Fun��o MAIN
int main()
{
	srand(glfwGetTime());

	// Inicializa��o da GLFW
	glfwInit();

	//Muita aten��o aqui: alguns ambientes n�o aceitam essas configura��es
	//Voc� deve adaptar para a vers�o do OpenGL suportada por sua placa
	//Sugest�o: comente essas linhas de c�digo para desobrir a vers�o e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

	//Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

	// Cria��o da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Sprites!!!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da fun��o de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);


	// GLAD: carrega todos os ponteiros d fun��es da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informa��es de vers�o
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	
	// Exerc�cio 4 da lista 2
	// glViewport(400, 300, 400, 300);


	// Compilando e buildando o programa de shader
	Shader shader("HelloTriangle.vs", "HelloTriangle.fs");

	// Gerando um buffer simples, com a geometria de um tri�ngulo
	int texWidth, texHeight;
	//GLuint texID = setupTexture("../../Textures/large_red_bricks_diff_1k.jpg", texWidth, texHeight);
	GLuint texID = setupTexture("../../Textures/desert-100.jpg", texWidth, texHeight);
	
	Sprite background;
	background.initialize(texID, glm::vec2(texWidth*0.25, texHeight*0.25),&shader);

	texID = setupTexture("../../Textures/flaming_meteor.png", texWidth, texHeight);
	Meteor meteor;
	meteor.initialize(texID, glm::vec2(texWidth*2, texHeight * 2), &shader,1, 1, glm::vec3(400.0,620.0,0.0));
	meteor.setVelocity(10.0);

	texID = setupTexture("../../Textures/dinoanda.png", texWidth, texHeight);
	dino.initialize(texID, glm::vec2(texWidth*2, texHeight*2), &shader,1, 5, glm::vec3(100.0,150.0,0.0));
	dino.setVelocity(7.5);

	Timer timer;

	glUseProgram(shader.ID);

	glActiveTexture(GL_TEXTURE0);
	shader.setTexBuffer0("texBuff");

	glm::mat4 projection = glm::ortho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);
	
	GLint projLoc = glGetUniformLocation(shader.ID, "projection");
	glUniformMatrix4fv(projLoc, 1, FALSE, glm::value_ptr(projection));

	//Habilitando a fun��o de mistura no color buffer - transpar�ncia
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Habilita o teste de profundidade
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);


	// Loop da aplica��o - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		timer.start();
		// Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as fun��es de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		background.update();
		background.draw();

		meteor.update();
		meteor.draw();

		dino.update();
		dino.draw();

		bool isColliding = testCollision(meteor, dino);
		//AQUI processa a l�gica da colis�o
		if (isColliding)
			cout << "BOOM!" << endl;

		//cout << dino.getPosition().x << " " << dino.getPosition().y << endl;

		timer.finish();
		double waitingTime = timer.calcWaitingTime(30, timer.getElapsedTimeMs());
		if (waitingTime > 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds((int)waitingTime));
		}

		// Troca os buffers da tela
		glfwSwapBuffers(window);

	}
	
	// Finaliza a execu��o da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Fun��o de callback de teclado - s� pode ter uma inst�ncia (deve ser est�tica se
// estiver dentro de uma classe) - � chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT)
	{
		dino.moveRight();
	}

	if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT)
	{
		dino.moveLeft();
	}

}

void mouse_callback(GLFWwindow* window, double mouse_x, double mouse_y)
{
	//cout << mouse_x << " " << mouse_y << endl;
}



// Esta fun��o est� bastante harcoded - objetivo � criar os buffers que armazenam a 
// geometria de um tri�ngulo
// Apenas atributo coordenada nos v�rtices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A fun��o retorna o identificador do VAO
int setupGeometry()
{
	// Aqui setamos as coordenadas x, y e z do tri�ngulo e as armazenamos de forma
	// sequencial, j� visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do v�rtice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO �nico ou em VBOs separados
	GLfloat vertices[] = {
		//x   y     z    r    g    b    s	t
		-0.5, -0.5, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,//v0
		 0.5, -0.5, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0,//v1
		 0.0,  0.5, 0.0, 0.0, 0.0, 1.0, 0.5, 1.0 //v2

	};

	GLuint VBO, VAO;

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

	return VAO;
}

int setupTexture(string texName, int& width, int& height)
{
	GLuint texID;

	// Gera o identificador da textura na mem�ria 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	//Parametrizando o wrapping da textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	//Carregamento do arquivo de imagem da textura
	int nrChannels;
	unsigned char* data = stbi_load(texName.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		//Enviar a informa��o dos pixels da imagem para OpenGL gerar o buffer de textura
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	return texID;
}

bool testCollision(Sprite &a, Sprite &b)
{
	AABB pA = a.getAABB();
	AABB pB = b.getAABB();

	//teste de colis�o
	//Fazer o if pra ver se um AABB n�o sobrep�e o outro
	// Colis�o no eixo x?
	bool collisionX = pA.pmin.x < pB.pmax.x && pA.pmax.x > pB.pmin.x;
	// Colis�o no eixo y?
	bool collisionY = pA.pmin.y < pB.pmax.y && pA.pmax.y > pB.pmin.y;
	
	//H� colis�o se ambas s�o verdadeiras
	return collisionX && collisionY;

}


