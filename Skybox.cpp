#include "Skybox.h"

Skybox::Skybox(std::string folderPath) {
	this->vaoID = 0;
	this->vboID = 0;
	this->textureID = 0;
	this->folderPath = folderPath;

	this->faces[0] = folderPath + "/right.jpg";
	this->faces[1] = folderPath + "/left.jpg";
	this->faces[2] = folderPath + "/top.jpg";
	this->faces[3] = folderPath + "/bottom.jpg";
	this->faces[4] = folderPath + "/front.jpg";
	this->faces[5] = folderPath + "/back.jpg";
}


Skybox::~Skybox(){

}
