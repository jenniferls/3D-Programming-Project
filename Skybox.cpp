#include "Skybox.h"
#include <Windows.h>
#include "glew/include/GL/glew.h"
#include <gl/GL.h>

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
	OutputDebugStringA("Destructor is run for Skybox\n");
	glDeleteVertexArrays(1, &vaoID);
	glDeleteBuffers(1, &vboID);
}

void Skybox::prepare(unsigned int& shaderProgram) {
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	glEnableVertexAttribArray(0);

	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);

	glBufferData(GL_ARRAY_BUFFER, sizeof(geom), geom, GL_STATIC_DRAW);

	GLint pos = glGetAttribLocation(shaderProgram, "position");

	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));
}
