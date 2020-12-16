#pragma once

#include "Gun.h"

class Shotgun final: public Gun{
public:
	Shotgun();
	void Shoot(const float& elapsedTime, const glm::vec3& camPos, const glm::vec3& camFront, ISoundEngine* const& soundEngine) override;
};