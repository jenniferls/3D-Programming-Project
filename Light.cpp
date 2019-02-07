#include "Light.h"

Light::Light(glm::vec3 position, glm::vec3 color) {
	this->pos = position;
	this->color = color;
	this->pos_id = -1;
	this->color_id = -1;
}

Light::~Light() {

}

glm::vec3 Light::getPosition() const {
	return this->pos;
}

glm::vec3 Light::getColor() const {
	return this->color;
}
