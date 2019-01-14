#include "GameTimer.h"
#include "glfw/include/GLFW/glfw3.h"

GameTimer::GameTimer() {
	this->deltaTime = 0.0f;
	this->lastFrame = 0.0f;
}

void GameTimer::Tick() {
	float currentFrame = glfwGetTime();
	this->deltaTime = currentFrame - this->lastFrame;
	this->lastFrame = currentFrame;
}

float GameTimer::GetDeltaTime() const{
	return deltaTime;
}
