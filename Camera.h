#pragma once
#include <windows.h>

#include "glfw/include/GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
	Camera(float screenWidth, float screenHeight, glm::vec3 position = glm::vec3(0.0f), float FoV = 45.0f, float speed = 7.5f);
	~Camera();

	void keyboardUpdate(float deltaTime);
	void updateFoV(float offset);
	void updateCameraVectors();

	float getCameraSpeed() const;
	float getFoV() const;

	glm::vec3 getCameraPosition() const;
	glm::vec3 getCameraUp() const;
	glm::vec3 getCameraFront() const;

	float getMouseLastX() const;
	float getMouseLastY() const;
	void setMouseLastX(float x);
	void setMouseLastY(float y);

	void calcCameraYaw(float offset);
	void calcCameraPitch(float offset);

private:
	glm::vec3 camPos; //Default camera position
	glm::vec3 camFront; //Default camera front
	glm::vec3 camUp; //Default camera up-vector
	float FoV; //Field-of-view
	float camYaw;
	float camPitch;
	float mouseLastX; //At centre of the screen
	float mouseLastY; //At centre of the screen
	float speed; //Camera movement speed

};