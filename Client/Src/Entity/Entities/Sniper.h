#pragma once

#include "Gun.h"

class Sniper final: public Gun{
public:
	Sniper();
	void Shoot(const float& elapsedTime, const glm::vec3& camPos, const glm::vec3& camFront, ISoundEngine* const& soundEngine) override;
};