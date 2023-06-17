/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle 
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 27/02/2023
 *
 */

#include <iostream>
#include <string>
#include <assert.h>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Outras 
#include <vector>
#include <fstream>

#include "Shader.h"
#include "stb_image.h"

using namespace std;


// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double mouse_x, double mouse_y);

// Protótipos das funções
int setupGeometry();//Cria a geometria do triângulo
int setup3DGeometry(); //Cria a geometria da pirâmide
int setupTexture(string texName, int& width, int& height);
int setupVoxel(glm::vec3 color);
void setupColorPalette();
void updateCameraPos(GLFWwindow* window);
void save(string fileName);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

//Variáveis de controle da câmera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); 
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//Variáveis globais para o controle da câmera
bool firstMouse = true;
float lastX = WIDTH / 2.0, lastY = HEIGHT / 2.0; //para calcular o quanto que o mouse deslocou
float yaw = -90.0, pitch = 0.0; //rotação em x e y

int voxelmap[5][5][5];
glm::vec3 gridCursor = glm::vec3(0, 0, 0);

vector <glm::vec3> colorPalette;
int iColor = 0;

enum colorNames {VERMELHO, VERDE, AZUL, AMARELO, MAGENTA, CIANO, PRETO, BRANCO, NONE = -1};


bool topview = false;

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	//Muita atenção aqui: alguns ambientes não aceitam essas configurações
	//Você deve adaptar para a versão do OpenGL suportada por sua placa
	//Sugestão: comente essas linhas de código para desobrir a versão e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

	//Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Voxels!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;


	// Compilando e buildando o programa de shader
	Shader shader("HelloTriangle.vs", "HelloTriangle.fs");

	//Definindo a paleta de cores
	setupColorPalette();

	glm::vec3 corAux = glm::vec3(1.0, 0.0, 0.0);
	// Gerando um buffer simples, com a geometria de um triângulo
	GLuint VAO = setupVoxel(corAux);
	
	int texWidth, texHeight;
	GLuint texID = setupTexture("../../Textures/large_red_bricks_diff_1k.jpg", texWidth, texHeight);
	
	glUseProgram(shader.ID);

	glActiveTexture(GL_TEXTURE0);

	shader.setTexBuffer0("texBuff");

	glm::mat4 view = glm::mat4(1);
		
	view = glm::lookAt(cameraPos, // Posição (ponto) 
					   glm::vec3(0.0f, 0.0f, 0.0f), // Target (ponto, não vetor) -> dir = target - pos                
					   cameraUp);// Up (vetor)

	GLint viewLoc = glGetUniformLocation(shader.ID, "view");
	glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));

	//glm::mat4 projection = glm::ortho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

	GLint projLoc = glGetUniformLocation(shader.ID, "projection");
	glUniformMatrix4fv(projLoc, 1, FALSE, glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	for (int x = 0; x < 5; x++)
		for (int y = 0; y < 5; y++)
			for (int z = 0; z < 5; z++)
			{
				voxelmap[x][y][z] = NONE;
			}

	glm::vec3 voxelDimensions = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 initialPos = glm::vec3(0.0, 0.0, 0.0);
	
	
	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{

		// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();
		updateCameraPos(window);

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);


		
		//Translação
		//float offsetZ = cos(glfwGetTime()) * 2.0f;
		//model = glm::translate(model, glm::vec3(0.0,0.0,offsetZ));

		//Rotação
		//float angle = glfwGetTime();
		//model = glm::rotate(model, angle, glm::vec3(0.0, 1.0, 0.0));
		
		//Escala
		//model = glm::scale(model, glm::vec3(200.0,200.0,1.0));

		if (!topview) {
			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		}
		else
		{
			glm::vec3 topViewPos = glm::vec3(0.0, 10.0, 0.0);
			glm::vec3 topViewUp = glm::vec3(0.0, 0.0, -1.0);

			view = glm::lookAt(topViewPos, // Posição (ponto) 
				glm::vec3(0.0f, 0.0f, 0.0f), // Target (ponto, não vetor) -> dir = target - pos                
				topViewUp);// Up (vetor)
		}

		shader.setMat4("view",glm::value_ptr(view));

		glBindVertexArray(VAO); //Conectando ao buffer de geometria desejado
		glBindTexture(GL_TEXTURE_2D, texID); //Conectando ao buffer de textura desejado
		

		//Matriz de modelo: transformações no objeto
		glm::mat4 model= glm::mat4(1); //matriz identidade


		

		//Desenhar o mapa
		for (int x = 0; x < 5; x++)
			for (int y = 0; y < 5; y++)
				for (int z = 0; z < 5; z++)
				{

					int iVoxel = voxelmap[x][y][z]; //recupera o indice do voxel no voxelMap
					if (iVoxel >= 0)
					{
						//mandar para o shader a cor do Voxel
						shader.setVec4("inputColor", colorPalette[iVoxel].r, colorPalette[iVoxel].g, colorPalette[iVoxel].b, 1.0);

						//alterar a posição do voxel
						model = glm::mat4(1); //matriz identidade

						glm::vec3 pos;
						pos.x = initialPos.x + x * voxelDimensions.x;
						pos.y = initialPos.y + y * voxelDimensions.y;
						pos.z = initialPos.z + z * voxelDimensions.z;

						model = glm::translate(model, glm::vec3(pos.x, pos.y, pos.z));

						//Enviando a matriz de modelo para o shader
						GLint modelLoc = glGetUniformLocation(shader.ID, "model");
						glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

						//Chamada de desenho do voxel
						glDrawArrays(GL_TRIANGLES, 0, 36);
					}
				}


				//Desenho do cursor
				shader.setVec4("inputColor", 1.0, 1.0, 0.0, 0.7);
				model = glm::mat4(1);

				glm::vec3 pos;
				pos.x = initialPos.x + gridCursor.x * voxelDimensions.x;
				pos.y = initialPos.y + gridCursor.y * voxelDimensions.y;
				pos.z = initialPos.z + gridCursor.z * voxelDimensions.z;

				model = glm::translate(model, glm::vec3(pos.x, pos.y, pos.z));
				model = glm::scale(model, glm::vec3(1.01, 1.01, 1.01));

				shader.setMat4("model", glm::value_ptr(model));
				//Chamada de desenho
				glDrawArrays(GL_TRIANGLES, 0, 36);


				//// Desenha o chão
				//glDrawArrays(GL_TRIANGLES, 18, 6);


				glBindVertexArray(0); //Desconectando o buffer de geometria

				// Troca os buffers da tela
				glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();


	save("cenaTeste.txt");

	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		gridCursor.z = ((int)gridCursor.z + 1) % 5;
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		gridCursor.z = ((int)gridCursor.z - 1 + 5) % 5;
	}

	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		gridCursor.x = ((int)gridCursor.x + 1) % 5;
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		gridCursor.x = ((int)gridCursor.x - 1 + 5) % 5;
	}

	if (key == GLFW_KEY_I && action == GLFW_PRESS)
	{
		gridCursor.y = ((int)gridCursor.y + 1) % 5;
	}
	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		gridCursor.y = ((int)gridCursor.y - 1 + 5) % 5;
	}

	if (key >= GLFW_KEY_0 && key <= GLFW_KEY_7 && action == GLFW_PRESS)
	{

		iColor = key - 48;
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		if (voxelmap[(int)gridCursor.x][(int)gridCursor.y][(int)gridCursor.z] != NONE)
		{
			//Remove o voxel
			voxelmap[(int)gridCursor.x][(int)gridCursor.y][(int)gridCursor.z] = NONE;
		}
		else
			//Insere o voxel com a cor selecionada
			voxelmap[(int)gridCursor.x][(int)gridCursor.y][(int)gridCursor.z] = iColor;
	}

	if (key == GLFW_KEY_V && action == GLFW_PRESS)
	{
		topview = !topview;
	}

	//cout << (int)gridCursor.x << " " << (int)gridCursor.y << " " << (int)gridCursor.z << endl;
}

void mouse_callback(GLFWwindow* window, double mouse_x, double mouse_y)
{
	//cout << mouse_x << " " << mouse_y << endl;
	if (firstMouse)
	{
		lastX = mouse_x;
		lastY = mouse_y;
		firstMouse = false;
	}

	float xoffset = mouse_x - lastX;
	float yoffset = lastY - mouse_y;
	lastX = mouse_x;
	lastY = mouse_y;

	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;   //rotação y
	pitch += yoffset; //rotação x

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

	//Precisamos também atualizar o cameraUp!! Pra isso, usamos o Up do  
	//mundo (y), recalculamos Right e depois o Up
	glm::vec3 right = glm::normalize(glm::cross(cameraFront,
		glm::vec3(0.0, 1.0, 0.0)));
	cameraUp = glm::normalize(glm::cross(right, cameraFront));

}



// Esta função está bastante harcoded - objetivo é criar os buffers que armazenam a 
// geometria de um triângulo
// Apenas atributo coordenada nos vértices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO
int setupGeometry()
{
	// Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
	// sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO único ou em VBOs separados
	GLfloat vertices[] = {
		//x   y     z    r    g    b    s	t
		-0.5, -0.5, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,//v0
		 0.5, -0.5, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0,//v1
		 0.0,  0.5, 0.0, 0.0, 0.0, 1.0, 0.5, 1.0 //v2

	};

	GLuint VBO, VAO;

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

	return VAO;
}

int setup3DGeometry()
{
	GLfloat vertices[] = {
		//Base da pirâmide: 2 triângulos
		//x    y    z    r    g    b
		-0.5, -0.5, -0.5, 1.0, 1.0, 0.0,
		-0.5, -0.5,  0.5, 0.0, 1.0, 1.0,
		 0.5, -0.5, -0.5, 1.0, 0.0, 1.0,
		-0.5, -0.5, 0.5, 1.0, 1.0, 0.0,
		 0.5, -0.5,  0.5, 0.0, 1.0, 1.0,
		 0.5, -0.5, -0.5, 1.0, 0.0, 1.0,
		//Triangulo 1
		-0.5, -0.5, -0.5, 1.0, 1.0, 0.0,
		 0.0,  0.5,  0.0, 1.0, 1.0, 0.0,
		 0.5, -0.5, -0.5, 1.0, 1.0, 0.0,
		//Triangulo 2
		-0.5, -0.5, -0.5, 1.0, 0.0, 1.0,
		 0.0,  0.5,  0.0, 1.0, 0.0, 1.0,
		-0.5, -0.5, 0.5, 1.0, 0.0, 1.0,
		//Triangulo 3
		-0.5, -0.5, 0.5, 1.0, 1.0, 0.0,
		 0.0,  0.5,  0.0, 1.0, 1.0, 0.0,
		 0.5, -0.5, 0.5, 1.0, 1.0, 0.0,
		//Triangulo 4
		 0.5, -0.5, 0.5,  0.0, 1.0, 1.0,
		 0.0,  0.5,  0.0, 0.0, 1.0, 1.0,
		 0.5, -0.5, -0.5, 0.0, 1.0, 1.0,
		 //Chão
		-10.0, -0.5, -10.0, 0.5, 0.5, 0.5,
		-10.0, -0.5,  10.0, 0.5, 0.5, 0.5,
		 10.0, -0.5, -10.0, 0.5, 0.5, 0.5,
		-10.0, -0.5,  10.0, 0.5, 0.5, 0.5,
		 10.0, -0.5,  10.0, 0.5, 0.5, 0.5,
		 10.0, -0.5, -10.0, 0.5, 0.5, 0.5

	};

	GLuint VBO, VAO;

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo coordenada de textura 
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(2);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;

}

int setupTexture(string texName, int& width, int& height)
{
	GLuint texID;

	// Gera o identificador da textura na memória 
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
		//Enviar a informação dos pixels da imagem para OpenGL gerar o buffer de textura
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

int setupVoxel(glm::vec3 color)
{
	GLfloat vertices[] = {
		//Base do cubo: 2 triângulos
		//x    y    z     r     g     b     s    t
		-0.5, -0.5, -0.5, 0.88, 0.15, 0.07, 0.0, 0.0,
		-0.5, -0.5,  0.5, 0.88, 0.15, 0.07, 0.0, 1.0,
		 0.5, -0.5, -0.5, 0.88, 0.15, 0.07, 1.0, 0.0,

		-0.5, -0.5,  0.5, 0.88, 0.15, 0.07, 1.0, 0.0,
		 0.5, -0.5,  0.5, 0.88, 0.15, 0.07, 0.0, 1.0,
		 0.5, -0.5, -0.5, 0.88, 0.15, 0.07, 1.0, 1.0,

		//Face de cima: 2 triângulos
		-0.5, 0.5,  0.5, 1.0, 0.41, 0.03, 0.0, 0.0,
		 0.5, 0.5,  0.5, 1.0, 0.41, 0.03, 0.0, 1.0,
		-0.5, 0.5, -0.5, 1.0, 0.41, 0.03, 1.0, 0.0,

		 0.5, 0.5,  0.5, 1.0, 0.41, 0.03, 1.0, 0.0,
		 0.5, 0.5, -0.5, 1.0, 0.41, 0.03, 0.0, 1.0,
		-0.5, 0.5, -0.5, 1.0, 0.41, 0.03, 1.0, 1.0,

		//Face de frente: 2 triângulos
		-0.5, -0.5, -0.5, 0.94, 1.0, 0.03, 0.0, 0.0,
		-0.5,  0.5, -0.5, 0.94, 1.0, 0.03, 0.0, 1.0,
		 0.5, -0.5, -0.5, 0.94, 1.0, 0.03, 1.0, 0.0,

		 0.5, -0.5, -0.5, 0.94, 1.0, 0.03, 1.0, 0.0,
		-0.5,  0.5, -0.5, 0.94, 1.0, 0.03, 0.0, 1.0,
		 0.5,  0.5, -0.5, 0.94, 1.0, 0.03, 1.0, 1.0,

		//Face de trás: 2 triângulos
		-0.5,  0.5, 0.5, 0.09, 0.49, 0.12, 0.0, 0.0,
		-0.5, -0.5, 0.5, 0.09, 0.49, 0.12, 0.0, 1.0,
		 0.5,  0.5, 0.5, 0.09, 0.49, 0.12, 1.0, 0.0,

		 0.5,  0.5, 0.5, 0.09, 0.49, 0.12, 1.0, 0.0,
		-0.5, -0.5, 0.5, 0.09, 0.49, 0.12, 0.0, 1.0,
		 0.5, -0.5, 0.5, 0.09, 0.49, 0.12, 1.0, 1.0,
		
		 //Face da esquerda: 2 triângulos
		-0.5,  0.5, -0.5, 0.28, 0.28, 1.0, 0.0, 0.0,
		-0.5, -0.5, -0.5, 0.28, 0.28, 1.0, 0.0, 1.0,
		-0.5, -0.5,  0.5, 0.28, 0.28, 1.0, 1.0, 0.0,

		-0.5, -0.5,  0.5, 0.28, 0.28, 1.0, 1.0, 0.0,
		-0.5,  0.5,  0.5, 0.28, 0.28, 1.0, 0.0, 1.0,
		-0.5,  0.5, -0.5, 0.28, 0.28, 1.0, 1.0, 1.0,
		
		//Face da direita: 2 triângulos
		0.5,  0.5,  0.5, 0.47, 0.18, 0.54, 0.0, 0.0,
		0.5, -0.5,  0.5, 0.47, 0.18, 0.54, 0.0, 1.0,
		0.5, -0.5, -0.5, 0.47, 0.18, 0.54, 1.0, 0.0,

		0.5, -0.5, -0.5, 0.47, 0.18, 0.54, 1.0, 0.0,
		0.5,  0.5, -0.5, 0.47, 0.18, 0.54, 0.0, 1.0,
		0.5,  0.5,  0.5, 0.47, 0.18, 0.54, 1.0, 1.0,
	};


	GLuint VBO, VAO;

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

	return VAO;

}


int setupVoxelGrid(int width, int height, int depth)
{
	GLfloat vertices[] = {
		//Base do cubo: 2 triângulos
		//x    y    z     r     g     b     s    t
		-0.5, -0.5, -0.5, 0.88, 0.15, 0.07, 0.0, 0.0,
		-0.5, -0.5,  0.5, 0.88, 0.15, 0.07, 0.0, 1.0,
		 //0.5, -0.5, -0.5, 0.88, 0.15, 0.07, 1.0, 0.0,

		//-0.5, -0.5,  0.5, 0.88, 0.15, 0.07, 1.0, 0.0,
		 0.5, -0.5,  0.5, 0.88, 0.15, 0.07, 0.0, 1.0,
		 0.5, -0.5, -0.5, 0.88, 0.15, 0.07, 1.0, 1.0,

		 //Face de cima: 2 triângulos
		 -0.5, 0.5,  0.5, 1.0, 0.41, 0.03, 0.0, 0.0,
		  0.5, 0.5,  0.5, 1.0, 0.41, 0.03, 0.0, 1.0,
		 -0.5, 0.5, -0.5, 1.0, 0.41, 0.03, 1.0, 0.0,

		  0.5, 0.5,  0.5, 1.0, 0.41, 0.03, 1.0, 0.0,
		  0.5, 0.5, -0.5, 1.0, 0.41, 0.03, 0.0, 1.0,
		 -0.5, 0.5, -0.5, 1.0, 0.41, 0.03, 1.0, 1.0,

		 //Face de frente: 2 triângulos
		 -0.5, -0.5, -0.5, 0.94, 1.0, 0.03, 0.0, 0.0,
		 -0.5,  0.5, -0.5, 0.94, 1.0, 0.03, 0.0, 1.0,
		  0.5, -0.5, -0.5, 0.94, 1.0, 0.03, 1.0, 0.0,

		  0.5, -0.5, -0.5, 0.94, 1.0, 0.03, 1.0, 0.0,
		 -0.5,  0.5, -0.5, 0.94, 1.0, 0.03, 0.0, 1.0,
		  0.5,  0.5, -0.5, 0.94, 1.0, 0.03, 1.0, 1.0,

		  //Face de trás: 2 triângulos
		  -0.5,  0.5, 0.5, 0.09, 0.49, 0.12, 0.0, 0.0,
		  -0.5, -0.5, 0.5, 0.09, 0.49, 0.12, 0.0, 1.0,
		   0.5,  0.5, 0.5, 0.09, 0.49, 0.12, 1.0, 0.0,

		   0.5,  0.5, 0.5, 0.09, 0.49, 0.12, 1.0, 0.0,
		  -0.5, -0.5, 0.5, 0.09, 0.49, 0.12, 0.0, 1.0,
		   0.5, -0.5, 0.5, 0.09, 0.49, 0.12, 1.0, 1.0,

		   //Face da esquerda: 2 triângulos
		  -0.5,  0.5, -0.5, 0.28, 0.28, 1.0, 0.0, 0.0,
		  -0.5, -0.5, -0.5, 0.28, 0.28, 1.0, 0.0, 1.0,
		  -0.5, -0.5,  0.5, 0.28, 0.28, 1.0, 1.0, 0.0,

		  -0.5, -0.5,  0.5, 0.28, 0.28, 1.0, 1.0, 0.0,
		  -0.5,  0.5,  0.5, 0.28, 0.28, 1.0, 0.0, 1.0,
		  -0.5,  0.5, -0.5, 0.28, 0.28, 1.0, 1.0, 1.0,

		  //Face da direita: 2 triângulos
		  0.5,  0.5,  0.5, 0.47, 0.18, 0.54, 0.0, 0.0,
		  0.5, -0.5,  0.5, 0.47, 0.18, 0.54, 0.0, 1.0,
		  0.5, -0.5, -0.5, 0.47, 0.18, 0.54, 1.0, 0.0,

		  0.5, -0.5, -0.5, 0.47, 0.18, 0.54, 1.0, 0.0,
		  0.5,  0.5, -0.5, 0.47, 0.18, 0.54, 0.0, 1.0,
		  0.5,  0.5,  0.5, 0.47, 0.18, 0.54, 1.0, 1.0,
	};


	GLuint VBO, VAO;

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

	return VAO;

}

void updateCameraPos(GLFWwindow* window)
{
	float cameraSpeed = 0.05f; // adjust accordingly


	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

}

void setupColorPalette()
{
	colorPalette.push_back(glm::vec3(1.0, 0.0, 0.0)); //vermelho 0
	colorPalette.push_back(glm::vec3(0.0, 1.0, 0.0)); //verde 1
	colorPalette.push_back(glm::vec3(0.0, 0.0, 1.0)); //azul 2
	colorPalette.push_back(glm::vec3(1.0, 1.0, 0.0)); //amarelo 3
	colorPalette.push_back(glm::vec3(1.0, 0.0, 1.0)); //magenta 4
	colorPalette.push_back(glm::vec3(0.0, 1.0, 1.0)); //ciano 5
	colorPalette.push_back(glm::vec3(0.0, 0.0, 0.0)); //preto 6
	colorPalette.push_back(glm::vec3(1.0, 1.0, 1.0)); //branco 7
}

void save(string fileName)
{
	ofstream outputFile;
	outputFile.open(fileName);

	//Salvar o mapa
	for (int y = 0; y < 5; y++) // linhas y
	{
		for (int z = 0; z < 5; z++) //profundidade z
		{
			for (int x = 0; x < 5; x++)//largura x
			{
				int iVoxel = voxelmap[x][y][z];
				outputFile << iVoxel << " ";

			}
			outputFile << endl;
		}
		outputFile << endl;
	}


	outputFile.close();

}
