#pragma once

// GLM
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class Sprite
{
public:
	Sprite() {}
	~Sprite();
	void initialize(GLuint texID, glm::vec2 sprDimensions, Shader *shader, glm::vec3 position = glm::vec3(400.0,300.0,0.0), glm::vec3 scale = glm::vec3(1.0,1.0,1.0), float angle = 0, int nAnimations = 1, int nFrames = 1, glm::vec3 axis = glm::vec3(0.0,0.0,1.0));
	void update();
	void draw();
protected:
	GLuint VAO; //identificador do buffer de geometria Vertex Array Object
	GLuint texID; //identificador do buffer de textura
	
	//Informações sobre as transformações da sprite
	glm::vec3 position, scale, axis;
	float angle;

	//Endereço do shader
	Shader* shader;

	//Propriedades do sprite/spritesheet
	glm::vec2 sprDimensions;
	int nAnimations, nFrames;

};

