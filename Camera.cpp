#include "Camera.h"

Camera::Camera(float screenWidth, float screenHeight, glm::vec3 position, float FoV, float speed) {
	this->camPos = position;
	this->camFront = glm::vec3(0.0f, 0.0f, -1.0f);
	this->camUp = glm::vec3(0.0f, 1.0f, 0.0f);
	this->FoV = FoV;
	this->camYaw = -90.0f;
	this->camPitch = 0.0f;
	this->mouseLastX = screenWidth / 2;
	this->mouseLastY = screenHeight / 2;
	this->speed = speed;
}

Camera::~Camera() {

}

void Camera::keyboardUpdate(float deltaTime) {
	float movementSpeed = this->speed * deltaTime;
	if (GetAsyncKeyState(GLFW_KEY_W)) //GetAsyncKeyState Determines whether a key is up or down at the time of the function call.
		this->camPos += movementSpeed * this->camFront;

	if (GetAsyncKeyState(GLFW_KEY_A))
		this->camPos -= glm::normalize(glm::cross(this->camFront, this->camUp)) * movementSpeed;

	if (GetAsyncKeyState(GLFW_KEY_S))
		this->camPos -= movementSpeed * this->camFront;

	if (GetAsyncKeyState(GLFW_KEY_D))
		this->camPos += glm::normalize(glm::cross(this->camFront, this->camUp)) * movementSpeed;
}

void Camera::updateFoV(float offset) {
	this->FoV = (this->FoV - 5 * offset);
	if (this->FoV <= 10.0f) {
		this->FoV = 10.0f;
	}
	if (this->FoV >= 100.0f) {
		this->FoV = 100.0f;
	}
}

void Camera::updateCameraVectors() {
	//Constraining camera movement to avoid flipping
	if (camPitch >= 89.0f) {
		camPitch = 89.0f;;
	}
	if (camPitch <= -89.0f) {
		camPitch = -89.0f;
	}

	//Calculating direction vector
	glm::vec3 direction = glm::vec3(
		cos(glm::radians(camPitch)) * cos(glm::radians(camYaw)),
		sin(glm::radians(camPitch)),
		cos(glm::radians(camPitch)) * sin(glm::radians(camYaw)));
	camFront = glm::normalize(direction);
}

float Camera::getCameraSpeed() const {
	return this->speed;
}

float Camera::getFoV() const {
	return this->FoV;
}

glm::vec3 Camera::getCameraPosition() const {
	return this->camPos;
}

glm::vec3 Camera::getCameraUp() const {
	return this->camUp;
}

glm::vec3 Camera::getCameraFront() const {
	return this->camFront;
}

float Camera::getMouseLastX() const {
	return this->mouseLastX;
}

float Camera::getMouseLastY() const {
	return this->mouseLastY;
}

void Camera::setMouseLastX(float x) {
	this->mouseLastX = x;
}

void Camera::setMouseLastY(float y) {
	this->mouseLastY = y;
}

void Camera::calcCameraYaw(float offset) {
	this->camYaw += offset;
}

void Camera::calcCameraPitch(float offset) {
	this->camPitch += offset;
}
