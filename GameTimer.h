#pragma once

class GameTimer {
public:
	GameTimer();

	void Tick();
	float GetDeltaTime() const;

private:
	float deltaTime; //Time between current frame and last frame
	float lastFrame; //Time of last frame
};