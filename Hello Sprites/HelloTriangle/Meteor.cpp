#include "Meteor.h"

void Meteor::update()
{
	if (position.y <= 150)
	{
		position.y = 620 + rand() % 31;
		position.x = sprDimensions.x / 2 + rand() % 800 - sprDimensions.x;
	}

	position.y -= vel;
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
